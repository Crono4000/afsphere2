
#include "afsphere.h"

char	*get_file_name(char	*path)
{
	char	*tok;
	char	*result;

	result = NULL;
	tok = strtok(path, "/");
	while (tok != NULL)
	{
		result = tok;
        tok = strtok(NULL, "/");
	}
	return (result);
}

long	power(long b, int p)
{
	if (p == 0)
		return (1);
	return (b * power(b, p - 1));
}

void	putnbr_on_string(char	*str, int index, unsigned long	n)
{
	if (n >= 10)
		putnbr_on_string(str, index - 1, n / 10);
	str[index] = '0' + (n % 10);
}

char	*ltoa(unsigned long	n)
{
	char	*result;
	int		size;

	size = 0;
	while (power(10, size) < n)
		size++;
	result = (char *) malloc(sizeof(char) * (size + 1));
	if (result != NULL)
	{
		putnbr_on_string(result, size - 1, n);
		result[size] = '\0';
	}
	return (result);
}

int	get_file_length(char *file1, off_t *length)
{
	struct stat st;
	int			error_code;

	error_code = 0;
	error_code = stat(file1, &st);
	if (error_code == -1)
	{
		perror("Error calling stat: ");
		error_code = 5;
	}
	*length = st.st_size;
	return (error_code);
}

int	copy_file(char *file1, char *file2)
{
	char	buffer[CPY_BUFFER];
	int		fd1;
	int		fd2;
	int		bytes_read;
	int		write_return;

	fd1 = open(file1, O_RDONLY);
	if (fd1 == -1)
		return (3);
	fd2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd2 == -1)
		return (close(fd1), 3);
	bytes_read = read(fd1, buffer, sizeof(buffer));
	while (bytes_read == sizeof(buffer))
	{
		write_return = write(fd2, buffer, bytes_read);
		if (write_return == -1)
			return (close(fd1), close(fd2), 1);
		bytes_read = read(fd1, buffer, sizeof(buffer));
	}
	if (bytes_read == -1)
		return (close(fd1), close(fd2), 2);
	if (bytes_read > 0)
	{
		write_return = write(fd2, buffer, bytes_read);
		if (write_return == -1)
			return (close(fd1), close(fd2), 1);
	}
	return (0);
}

int	get_file_content(char **content, char *file)
{
	int		fd;
	long	size;
	int		content_indice;
	int		bytes_read;
	int		error_code;

	content_indice = 0;
	error_code = get_file_length(file, (off_t *) &size);
	if (error_code)
		return (error_code);
	*content = malloc(sizeof(char) * (size + 1));
	if (*content == NULL)
		return (4);
	fd = open(file, O_RDONLY);
	if (fd == -1)
		return (free(*content), *content = NULL, 3);
	bytes_read = read(fd, *content + content_indice, CPY_BUFFER);
	while (bytes_read == CPY_BUFFER)
	{
		content_indice += bytes_read;
		bytes_read = read(fd, *content + content_indice, CPY_BUFFER);
	}
	if (bytes_read == -1)
		return (close(fd), free(*content), *content = NULL, 2);
	if (bytes_read > 0)
		content_indice += bytes_read;
	(*content)[content_indice] = '\0';
	close(fd);
	return (0);
}
