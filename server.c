
#include "afsphere2.h"

static enum MHD_Result check_client(void *cls, const struct sockaddr *addr, socklen_t addrlen)
{
    printf("Alguem esta a tentar ligar\n");

    return MHD_YES;
}

static enum MHD_Result handle_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **req_cls)
{
    
}

int	start_server()
{
	struct MHD_Daemon	*daemon;
	apc_config_t		*apc;
	pid_t				pid;

	apc = malloc(sizeof(apc_config_t));
	if (apc == NULL)
		return (4);
	daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, 10000, &check_client, (void*)apc, &handle_request, void *dh_cls, MHD_OPTION_END);
	if (daemon == NULL)
		return (free(apc), 10);
	pid = fork();
	if (pid < 0)
		return (free(apc), 11);
	if (pid != 0)
	{
		prinf("The pid from server is %d\n", pid);
		
	}
	return (0);
}

