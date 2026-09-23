
#include "afsphere.h"

static app_t *app;

int	queue_pipe_response(struct MHD_Connection *connection, int **pipe_fds)
{
    struct MHD_Response *response;
    enum MHD_Result     result;

	if (pipe(*pipe_fds) == -1)
		return (15);
    response = MHD_create_response_from_pipe((*pipe_fds)[0]);
    if (response == NULL)
        return (16);
    result = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    if (result != MHD_YES)
    {
        close((*pipe_fds)[1]);
        free(*pipe_fds);
        *pipe_fds = NULL;
        return (14);
    }
    return 0;
}

int queue_page_sql(struct MHD_Connection *connection, char *file, char *template, char *query, char **args, int size)
{
	int					pipe[2];
	enum MHD_Result     result;
	int					error_code;

	error_code = queue_pipe_response(connection, &pipe);
	if (error_code)
		return (error_code);
	error_code = render_page_sql(conn, pipe[1], file, template, query, args, size);
	return error_code;
}

static enum MHD_Result check_client(void *cls, const struct sockaddr *addr, socklen_t addrlen)
{
    printf("Alguem esta a tentar ligar\n");
    return MHD_YES;
}

static enum MHD_Result handle_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **req_cls)
{
    int		error_code;
	char	*search;
	char	*isfile;
	char	*page;

	search = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "search");
	isfile = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "isfile");
	page = MHD_lookup_connection_value(connection, MHD_GET_ARGUMENT_KIND, "page");
	if (strcmp(url, "/search_infos") == 0 && isfile != NULL && search != NULL && page != NULL && (strcmp(isfile, "true") == 0 || strcmp(isfile, "false") == 0))
		error_code = queue_page_sql(connection, fd, file, template, query, str_array(3, search, isfile, page), 3);
	if (error_code)
		return (MHD_NO);
	return (MHD_YES);
}

void finish_process(int signal)
{
	MHD_stop_daemon(app->daemon);
	PQfinish(app->conn);
	free(app);
	exit(0);
}

int write_pid(pid_t pid)
{
	int	fd;

	fd = open("/var/run/afsphere.pid", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd <= 0)
		return (3);
	if (write(fd, &pid, sizeof(pid_t)) < 0)
		return (close(fd), 1);
	close(fd);
	return (0);
}

int read_pid(pid_t *pid2)
{
	pid_t	pid;
	int		fd;

	fd = open("/var/run/afsphere.pid", O_RDONLY);
	if (fd <= 0)
		return (3);
	if (read(fd, &pid, sizeof(pid_t)) < 0)
		return (close(fd), 2);
	*pid2 = pid;
	close(fd);
	return (0);
}

int	start_server(PGconn	*conn)
{
	pid_t				pid;

	pid = fork();
	if (pid < 0)
		return (11);
	if (pid != 0)
	{
		printf("The pid from server is %d\n", pid);
		return (write_pid(pid));
	}
	app = malloc(sizeof(app_t));
	if (app == NULL)
		return (4);
	app->conn = conn;
	app->daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, 8080, &check_client, NULL, &handle_request, NULL, MHD_OPTION_END);
	if (daemon == NULL)
		return (free(app), 10);
	setsid();
	signal(SIGTERM, finish_process);
	pause();
}

int	stop_server()
{
	pid_t	pid;
	int		error;

	error = read_pid(&pid);
	if (error)
		return (error);
	printf("pid:%d\n", pid);
	kill(pid, SIGTERM);
	return (0);
}
