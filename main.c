
#include "afsphere.h"

void	print_error(int error_code)
{
	if (error_code == 1)
		perror("Error on write: ");
	if (error_code == 2)
		perror("Error on read: ");
	if (error_code == 3)
		perror("Error on open: ");
	if (error_code == 4)
		perror("Error on malloc: ");
	if (error_code == 5)
		perror("Error on stat: ");
}

int main(int argc, char **argv)
{
    PGconn	*conn;
	char	*printable;
	long	*lprintable;
	int		error_code;

	printable = NULL;
	*lprintable = -1;
	conn = PQconnectdb("host=localhost port=5432 dbname=afsphere user=pizzamozzarella password=gotica");
	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Erro: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		return (1);
	}
	if (strcmp(argv[1], "execute_sql") == 0)
		exec_file_db(conn, argv[2]);
	if (strcmp(argv[1], "add_disk") == 0)
		add_disk_db(conn, argv[2], argv[3], argv[4]);
	if (strcmp(argv[1], "copy") == 0)
		if(copy_file(argv[2], argv[3]) == -1)
			perror("Error copying a file: ");
	if (strcmp(argv[1], "test_get_file_name") == 0)
		printf("filename:%s\n", get_file_name(argv[2]));
	if (strcmp(argv[1], "test_file_length") == 0)
		get_file_length(argv[2], lprintable);
	if (strcmp(argv[1], "test_cat") == 0)
		error_code = get_file_content(&printable, argv[2]);
	if (error_code)
		print_error(error_code);
	if (printable != NULL)
	{
		printf("%s\n", printable);
		free(printable);
	}
	if (*lprintable != -1)
		printf("filelength:%ld\n", *lprintable);
	PQfinish(conn);
	return (error_code);
}
