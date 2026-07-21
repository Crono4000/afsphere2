
#include "afsphere.h"

int	exec_file_db(PGconn *conn, char* file)
{
	char		*content;
	PGresult	*res;
	int			return_code;

	return_code = get_file_content(&content, file);
	if (return_code)
		return (return_code);
	res = PQexec(conn, content);
	return_code = PQresultStatus(res) == PGRES_COMMAND_OK ? 0 : 1;
	PQclear(res);
	free(content);
	return (return_code);
}

int	add_disk_db(PGconn *conn, char* disk, char	*limit, char *priority)
{
	char		**params;
	PGresult	*res;
	int			return_code;

	if (disk[0] != '/')
		return (1);
	params = malloc(sizeof(char *) * 4);
	params[0] = disk;
	params[1] = limit;
	params[2] = priority;
	params[3] = NULL;
	res = PQexecParams(conn, "INSERT INTO disk(disk_path, disk_limit, disk_priority) VALUES ($1, $2, $3);", 3, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_COMMAND_OK ? 0 : 1;
	free(params);
	PQclear(res);
	return (return_code);
}

int	move_file_to_db(PGconn *conn, char* file, char* file_id)
{
	char		**params;
	PGresult	*res;
	int			return_code;

	params = malloc(sizeof(char *) * 2);
	params[0] = file_id;
	params[1] = NULL;
	res = PQexecParams(conn, "SELECT file_path FROM file WHERE file_id = $1", 1, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 1;
	if (return_code == 0)
		return_code = copy_file(file, PQgetvalue(res, 0, 0));
	free(params);
	PQclear(res);
	return (return_code);
}

int	add_file_db(PGconn *conn, char* file)
{
	char		**params;
	PGresult	*res;
	long		filelength;
	int			return_code;

	return_code = get_file_length(file, &filelength);
	if (return_code)
		return (return_code);
	params = malloc(sizeof(char *) * 3);
	params[0] = get_file_name(file);
	params[1] = ltoa(filelength);
	params[2] = NULL;
	res = PQexecParams(conn, "CALL insert_file($1, $2)", 2, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_COMMAND_OK ? 0 : 1;
	free(params[0]);
	free(params[1]);
	free(params);
	PQclear(res);
	if (return_code == 0)
	{
		res = PQexec(conn, "SELECT MAX(file_id) FROM file");
		return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 1;
		if (return_code == 0)
			return_code = move_file_to_db(conn, file, PQgetvalue(res, 0, 0));
		PQclear(res);
	}
	return (return_code);
}
