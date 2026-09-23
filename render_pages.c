
#include "afsphere.h"

void    write_template(int fd, char **strings, char *str)
{
	int i;
	int index;

	i = 0;
	while (str[i])
	{
		if (str[i] == '$' && str[i + 1] >= '0' && str[i + 1] <= '9')
		{
			index = str[i + 1] - '0';
			write(fd, strings[index], strlen(strings[index]));
			i += 2;
		}
		else
		{
			write(fd, &str[i], 1);
			i++;
		}
	}
}

void	write_pgresult(PGresult *result, char *template, int fd)
{
	int		rows;
	int		fields;
	int		row;
	int		field;
	char	**values;

	rows = PQntuples(result);
	fields = PQnfields(result);
    values = malloc(sizeof(char *) * fields);
	row = 0;
	while (row < rows)
	{
		field = 0;
		while (field < fields)
		{
			values[field] = PQgetvalue(result, row, field);
			field++;
		}
		write_template(fd, values, template);
		row++;
	}
    free(values);
}

int	write_page_with_result(int fd, char *file, PGresult *result, char *template)
{
	char	read_buffer[CPY_BUFFER + 1];
	int		read_fd;
	int		read_bytes;
	int		read_index;

	read_fd = open(file, O_RDONLY);
	if (read_fd == -1)
		return (3);
	read_bytes = read(read_fd, read_buffer, CPY_BUFFER);
	read_buffer[read_bytes] = '\0';
	while (read_bytes)
	{
		read_index = 0;
		while (read_index < read_bytes)
		{
			if (read_buffer[read_index] == '$' && read_buffer[read_index + 1] == '$')
			{
				write_pgresult(result, template, fd);
				read_index++;
			}
			else
				write(fd, read_buffer + read_index, 1);
			read_index++;
		}
		read_bytes = read(read_fd, read_buffer, CPY_BUFFER);
		read_buffer[read_bytes] = '\0';
	}
	close(fd);
}

int	render_page_sql(PGconn *conn, int fd, char *file, char *template, char *query, char **args, int size)
{
	PGresult	*res;
	int			return_code;

	res = PQexecParams(conn, query, size, NULL, args, NULL, NULL, 0);
	return_code = PQresultStatus(res) == PGRES_TUPLES_OK ? 0 : 7;
	if (return_code == 0)
		return_code = write_page_with_result(fd, file, res, template);
	return (return_code);
}
