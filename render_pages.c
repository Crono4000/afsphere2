
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
    close(fd);
}

//void    write_info_table_page


