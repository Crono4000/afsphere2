
#include "afsphere.h"

int main(int argc, char **argv)
{
    PGconn *conn;

    conn = PQconnectdb("host=localhost port=5432 dbname=afsphere user=pizzamozzarella password=gotica");
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Erro: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return (1);
    }
    if (strcmp(argv[1], "copy") == 0)
		if(copy_file(argv[2], argv[3]) == -1)
			perror("Error copying a file: ");
	if (strcmp(argv[1], "test") == 0)
		printf("%s\n", ltoa(129990));
    PQfinish(conn);
    return 0;
}
