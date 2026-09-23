
#include "afsphere.h"

void	print_error(int error_code, PGconn	*conn)
{
	printf("error code:%d\n", error_code);
	if (error_code == 1)
		perror("Error on write");
	if (error_code == 2)
		perror("Error on read");
	if (error_code == 3)
		perror("Error on open");
	if (error_code == 4)
		perror("Error on malloc");
	if (error_code == 5)
		perror("Error on stat");
	if (error_code == 6)
		printf("Invalid syntax: %s\n", "The number of arguments was invalid or the command didn't exist");
	if (error_code == 7)
		printf("Calling sql query %s\n", PQerrorMessage(conn));
	if (error_code == 8)
		printf("Connecting to psql %s\n", PQerrorMessage(conn));
	if (error_code == 9)
		printf("Error getting the name of the path");
	if (error_code == 10)
		perror("Error starting the server.\n");
	if (error_code == 11)
		perror("Error forking the server.\n");
	if (error_code == 12)
		printf("You need to be root to execute this command.\n");
	if (error_code == 13)
		printf("The length file was higher than the malloc limit.\n");
	if (error_code == 14)
		printf("A function from microhttplib returned MHD_NO\n");
	if (error_code == 15)
		perror("Error on pipe");
	if (error_code == 16)
		perror("Error initializing a response in microhttplolib");
}

int main(int argc, char **argv)
{
	PGconn	*conn;
	char	*printable;
	long	lprintable;
	int		error_code;

	error_code = 0;
	printable = NULL;
	lprintable = -1;
	conn = PQconnectdb("host=localhost port=5432 dbname=afsphere user=pizzamozzarella password=gotica");
	if (PQstatus(conn) != CONNECTION_OK)
		error_code = 8;
	if (!error_code)
	{
		if (argc == 3 && strcmp(argv[1], "execute_sql") == 0)
			error_code = exec_file_db(conn, argv[2]);
		else if (argc == 4 && strcmp(argv[1], "copy") == 0)
			error_code = copy_file(argv[2], argv[3]);
		else if (argc >= 4 && strcmp(argv[1], "add_content") == 0)
			error_code = add_content_db(conn, argv[2], argv[3], argv + 4, argc - 4);
		else if (argc >= 3 && strcmp(argv[1], "add_tags") == 0)
			error_code = add_info_tags_db(conn, argv[2], argv + 3, argc - 3);
		else if (argc == 3 && strcmp(argv[1], "add_file") == 0)
			error_code = add_file_db(conn, argv[2]);
		else if (argc == 3 && strcmp(argv[1], "download_info") == 0)
			error_code = put_info_into_file_db(conn, argv[2]);
		else if (argc == 2 && strcmp(argv[1], "format") == 0)
		{
			if (geteuid() == 0)
				error_code = format_db(conn);
			else
				error_code = 12;
		}
		else if (argc == 2 && strcmp(argv[1], "start_server") == 0)
		{
			if (geteuid() == 0)
				error_code = start_server(conn);
			else
				error_code = 12;
		}
		else if (argc == 2 && strcmp(argv[1], "end_server") == 0)
		{
			if (geteuid() == 0)
				error_code = stop_server();
			else
				error_code = 12;
		}
		else if (argc == 3 && strcmp(argv[1], "show_info") == 0)
			error_code = show_info_db(conn, argv[2]);
		else if (argc == 3 && strcmp(argv[1], "show_query") == 0)
			error_code = show_query_db(conn, argv[2]);
		else if (argc >= 2 && strcmp(argv[1], "search_infos") == 0)
			error_code = search_info_db(conn, argv + 2, argc - 2);
		else if (argc >= 2 && strcmp(argv[1], "test_split_join") == 0)
			error_code = split_join(argv + 2, argc - 2, ',', '{', '}', &printable);
		else if (argc == 3 && strcmp(argv[1], "test_get_file_name") == 0)
			printf("filename:%s\n", get_file_name(argv[2]));
		else if (argc == 3 && strcmp(argv[1], "test_file_content") == 0)
			error_code = get_file_content(&printable, argv[2]);
		else if (argc == 3 && strcmp(argv[1], "test_file_length") == 0)
			error_code = get_file_length(argv[2], &lprintable);
		else if (argc == 3 && strcmp(argv[1], "test_cat") == 0)
			error_code = get_file_content(&printable, argv[2]);
		else
			error_code = 6;
	}
	if (error_code)
		print_error(error_code, conn);
	if (printable != NULL)
	{
		printf("%s\n", printable);
		free(printable);
	}
	if (lprintable != -1)
		printf("filelength:%ld\n", lprintable);
	PQfinish(conn);
	return (error_code);
}
