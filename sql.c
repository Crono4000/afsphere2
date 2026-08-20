
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
	return_code = PQresultStatus(res) == PGRES_COMMAND_OK ? 0 : 7;
	PQclear(res);
	free(content);
	return (return_code);
}

int	format_db(PGconn *conn)
{
	int	return_code;

	return_code = exec_file_db(conn, "psql/delete_all.sql");
	if (return_code)
		return (return_code);
	return_code = exec_file_db(conn, "psql/init_tables.sql");
	if (return_code)
		return (return_code);
	return_code = exec_file_db(conn, "psql/init_procedures.sql");
	return return_code;
}

int	add_disk_db(PGconn *conn, char* disk, char	*limit, char *priority)
{
	char		**params;
	PGresult	*res;
	int			return_code;

	if (disk[0] != '/')
		return (9);
	params = malloc(sizeof(char *) * 4);
	if (params == NULL)
		return (4);
	params[0] = disk;
	params[1] = limit;
	params[2] = priority;
	params[3] = NULL;
	res = PQexecParams(conn, "INSERT INTO disk(disk_path, disk_limit, disk_priority) VALUES ($1, $2, $3);", 3, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_COMMAND_OK ? 0 : 7;
	free(params);
	PQclear(res);
	return (return_code);
}

int	show_query_db(PGconn *conn, char *query)
{
	PGresult	*res;
	int			return_code;
	int			rows;
	int			cols;
	int			icol;
	int			irow;

	irow = 0;
	res = PQexec(conn, query);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	if (return_code != 7)
	{
		rows = PQntuples(res);
		cols = PQnfields(res);
		while (irow < rows)
		{
			icol = 0;
			while (icol < cols)
			{
				printf("%s ", PQgetvalue(res, irow, icol));
				icol++;
			}
			printf("\n");
			irow++;
		}
	}
	return (return_code);
}

int	move_file_to_db(PGconn *conn, char* file, char* file_id)
{
	char		**params;
	PGresult	*res;
	int			return_code;

	params = malloc(sizeof(char *) * 2);
	if (params == NULL)
		return (4);
	params[0] = file_id;
	params[1] = NULL;
	res = PQexecParams(conn, "SELECT file_path FROM file WHERE file_id = $1", 1, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
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
	char		*file_id;

	return_code = get_file_length(file, &filelength);
	if (return_code)
		return (return_code);
	params = malloc(sizeof(char *) * 3);
	if (params == NULL)
		return (4);
	params[0] = get_file_name(file);
	params[1] = ltoa(filelength);
	params[2] = NULL;
	res = PQexecParams(conn, "CALL insert_file($1, $2, NULL);", 2, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	free(params[1]);
	free(params);
	if (return_code == 0)
	{
		file_id = PQgetvalue(res, 0, 0);
		return_code = move_file_to_db(conn, file, file_id);
	}
	PQclear(res);
	return (return_code);
}

/*int	search_info_db(PGconn *conn, char *info_id, char **tags, int size)
{

}*/

int	add_info_tag_db(PGconn *conn, char *info_id, char *tag)
{
	char		**params;
	PGresult	*res;
	int			return_code;

	params = malloc(sizeof(char *) * 2);
	if (params == NULL)
		return (4);
	params[0] = tag;
	params[1] = info_id;
	res = PQexecParams(conn, "CALL insert_info_tag($1, $2);", 2, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_COMMAND_OK ? 0 : 7;
	free(params);
	return return_code;
}

int add_info_tags_db(PGconn *conn, char *info_id, char **tags, int size)
{
	int	ii;
	int	return_code;

	return_code = 0;
	ii = 0;
	while (ii < size)
	{
		return_code = add_info_tag_db(conn, info_id, tags[ii]);
		if (return_code)
			break;
		ii++;
	}
	return return_code;
}

int	add_content_db(PGconn *conn, char *title, char* content, char **tags, int size)
{
	int		return_code;
	char	*cont_id;

	return_code = add_info_bytea_db(conn, title, (void *) content, (long) strlen(content), "FALSE", &cont_id);
	if (!return_code)
		add_info_tags_db(conn, cont_id, tags, size);
	return (return_code);
}

int	add_info_bytea_db(PGconn *conn, char *title, void* content, long size, char *is_file, char **file_id)
{
	char		**params;
	PGresult	*res;
	int			return_code;
	const int	paramFormats[] = {0, 1, 0, 0};
	const int	paramLengths[] = {0, (int)size, 0, 0};

	params = malloc(sizeof(char *) * 4);
	if (params == NULL)
		return (4);
	params[0] = title;
	params[1] = content;
	params[2] = ltoa(size);
	params[3] = is_file;
	res = PQexecParams(conn, "CALL insert_info_bytea($1, $2, $3, $4, NULL);", 4, NULL, (const char *const*)params, paramLengths, paramFormats, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	free(params[2]);
	free(params);
	if(return_code == 0 && file_id)
		*file_id = PQgetvalue(res, 0, 0);
	return (return_code);
}
