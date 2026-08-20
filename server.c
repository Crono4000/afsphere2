
#include "afsphere.h"

static app_t *app;

static enum MHD_Result check_client(void *cls, const struct sockaddr *addr, socklen_t addrlen)
{
    printf("Alguem esta a tentar ligar\n");
    return MHD_YES;
}

static enum MHD_Result handle_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **req_cls)
{
    struct MHD_Response	*response;
	enum MHD_Result		result;
	char				*body;

	body = "Ola o website esta on.";
	response = MHD_create_response_from_buffer(strlen(body), (void*)body, MHD_RESPMEM_PERSISTENT);
	if (response == NULL)
		return MHD_NO;
	result = MHD_queue_response(connection, MHD_HTTP_OK, response);
	return result;
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
