
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
	if (return_code)
		return (return_code);
	return_code = exec_file_db(conn, "psql/init_functions.sql");
	return return_code;
}

int	print_query_res_db(PGresult	*res)
{
	int	rows;
	int	cols;
	int	icol;
	int	irow;

	irow = 0;
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
	return (0);
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
		print_query_res_db(res);
	}
	return (return_code);
}

int	search_info_db(PGconn *conn, char **tags, int size)
{
	PGresult	*res;
	int			return_code;
	char		*stags;

	return_code = split_join(tags, size, ',', '{', '}', &stags);
	if (return_code)
		return (return_code);
	res = PQexecParams(conn, "SELECT search_infos_by_tags($1::text[]);", 1, NULL, (const char *const*)&stags, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	if (return_code == 0)
		return_code = print_query_res_db(res);
	return (return_code);
}

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
			break ;
		ii++;
	}
	return (return_code);
}

int	create_info_with_oid_db(PGconn *conn, char *title, char *is_file, long size, char **file_id, Oid *oid)
{
	char		**params;
	PGresult	*res;
	int			return_code;

	params = malloc(sizeof(char *) * 3);
	if (params == NULL)
		return (4);
	params[0] = title;
	params[1] = ltoa(size);
	params[2] = is_file;
	res = PQexecParams(conn, "CALL insert_info_oid($1, $2, $3, NULL, NULL)", 3, NULL, (const char *const*)params, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	free(params[1]);
	free(params);
	if(return_code == 0 && file_id)
		*file_id = PQgetvalue(res, 0, 0);
	if(return_code == 0 && oid)
		*oid = (Oid)strtoul(PQgetvalue(res, 0, 1), NULL, 10);
	return (return_code);
}

int	add_content_db(PGconn *conn, char *title, char* content, char **tags, int size)
{
	int		return_code;
	char	*cont_id;

	return_code = add_info_bytea_db(conn, title, (void *) content, (long) strlen(content), "FALSE", &cont_id);
	if (return_code == 0)
		return_code = add_info_tags_db(conn, cont_id, tags, size);
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

int	copy_file_oid_db(PGconn *conn, char *file1, Oid oid)
{
	char	*buffer;
	off_t	length;
	int		fd1;
	int		fd2;
	ssize_t	bytes_read;
	int		write_return;
	int		return_code;

	return_code = get_file_length(file1, &length);
	if (return_code != 0)
		return (return_code);
	if (length < 0 || (uintmax_t)length > SIZE_MAX)
		return (13);
	buffer = NULL;
	if (length > 0)
	{
		buffer = malloc((size_t)length);
		if (buffer == NULL)
			return (4);
	}
	PQexec(conn, "BEGIN");
	fd1 = open(file1, O_RDONLY);
	if (fd1 == -1)
		return (free(buffer), PQexec(conn, "COMMIT"), 3);
	fd2 = lo_open(conn, oid, INV_WRITE);
	if (fd2 == -1)
		return (close(fd1), free(buffer), PQexec(conn, "COMMIT"), 3);
	if (length > 0)
	{
		bytes_read = read(fd1, buffer, (size_t)length);
		if (bytes_read != length)
			return (close(fd1), lo_close(conn, fd2), free(buffer), PQexec(conn, "ROLLBACK"), PQexec(conn, "COMMIT"), 2);
		write_return = lo_write(conn, fd2, buffer, (size_t)length);
		if (write_return != length)
			return (close(fd1), lo_close(conn, fd2), free(buffer), PQexec(conn, "ROLLBACK"), PQexec(conn, "COMMIT"), 1);
	}
	close(fd1);
	lo_close(conn, fd2);
	free(buffer);
	PQexec(conn, "COMMIT");
	return (0);
}

int	print_oid_file_db(PGconn *conn, int fd, Oid oid, long size)
{
	char	buffer[size];
	int		fd1;
	int		bytes_read;
	int		write_return;

	fd1 = lo_open(conn, oid, INV_READ);
	if (fd1 == -1)
		return (3);
	bytes_read = lo_read(conn, fd1, buffer, sizeof(buffer));
	if (bytes_read == -1)
		return (lo_close(conn, fd1), 2);
	write_return = write(fd, buffer, size);
	if (write_return == -1)
		return (lo_close(conn, fd1), 1);
	lo_close(conn, fd1);
	return (0);
}

int	get_info_db(PGconn *conn, info_t **info, char *file_id)
{
	PGresult	*res;
	int			return_code;
	char		*pick;

	*info = malloc(sizeof(info_t));
	if (*info == NULL)
		return (4);
	(*info)->flags = 0;
	res = PQexecParams(conn, "SELECT is_bytea, title, info_OID, info_size, is_file FROM info WHERE info_id = $1;", 1, NULL, (const char *const*) &file_id, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	if (return_code == 0)
	{
		pick = PQgetvalue(res, 0, 0);
		(*info)->flags = pick[0] == 't' ? FLAG_ISBYTEA : 0;
		(*info)->title = PQgetvalue(res, 0, 1);
		pick = PQgetvalue(res, 0, 2);
		(*info)->oid = (Oid)strtoul(pick, NULL, 10);
		pick = PQgetvalue(res, 0, 3);
		(*info)->size = (long) strtoul(pick, NULL, 10);
		pick = PQgetvalue(res, 0, 4);
		(*info)->flags = (pick[0] == 't' ? ((*info)->flags | FLAG_ISFILE) : (*info)->flags);
		if ((*info)->flags & FLAG_ISBYTEA)
		{
			res = PQexecParams(conn, "SELECT info_bytea FROM info WHERE info_id = $1;", 1, NULL, (const char *const*) &file_id, NULL, NULL, 1);
			return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
			if (return_code == 0)
				(*info)->bytea = PQgetvalue(res, 0, 0);
		}
	}
	else
		free(*info);
	return (return_code);
}

int	show_info_db(PGconn *conn, char *file_id)
{
	PGresult	*res;
	int			return_code;
	info_t		*info;

	return_code = get_info_db(conn, &info, file_id);
	if (return_code == 0)
	{
		printf("%s\n\n", info->title);
		if (info->flags & FLAG_ISBYTEA)
			write(1, info->bytea, info->size);
		else
			return_code = print_oid_file_db(conn, 1, info->oid, info->size);
		printf("%c", '\n');
		free(info);
	}
	return (return_code);
}

int	put_info_into_file_db(PGconn *conn, char *file_id)
{
	PGresult	*res;
	int			return_code;
	info_t		*info;
	int			fd;

	return_code = get_info_db(conn, &info, file_id);
	if (return_code == 0)
	{
		fd = open(info->title, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		if (fd == -1)
			return (free(info), 3);
		if (info->flags & FLAG_ISBYTEA)
			write(fd, info->bytea, info->size);
		else
			return_code = print_oid_file_db(conn, fd, info->oid, info->size);
		free(info);
		close(fd);
	}
	return (return_code);
}

int add_file_db(PGconn *conn, char *file)
{
	long	size;
	int		return_code;
	char	*content;
	char	*title;
	Oid		oid;

	return_code = get_file_length(file, &size);
	if (return_code)
		return (return_code);
	title = get_file_name(file);
	if (title == NULL)
		return (9);
	if (size < BYTEA_LIMIT)
	{
		return_code = get_file_content(&content, file);
		if (return_code)
			return (return_code);
		return_code = add_info_bytea_db(conn, title, (void *) content, size, "TRUE", NULL);
		free(content);
	}
	else
	{
		return_code = create_info_with_oid_db(conn, title, "TRUE", size, NULL, &oid);
		if (return_code)
			return (return_code);
		return_code = copy_file_oid_db(conn, file, oid);
	}
	return (return_code);
}
