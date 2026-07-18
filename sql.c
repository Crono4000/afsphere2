
#include <afsphere.h>

int	add_file_db(PGconn *conn, char* file)
{
	

	PGresult *res = PQexecParams(
		conn,
		"SELECT name, age FROM users WHERE id = $1",
		1,
		NULL,
		params,
		NULL,
		NULL,
		0
	);
}
