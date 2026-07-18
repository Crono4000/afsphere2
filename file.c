
#include "afsphere.h"

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

	size = 1;
	while (power(10, size) < n)
		size++;
	result = (char *) malloc(sizeof(char) * size);
	if (result != NULL)
	{
		putnbr_on_string(result, size - 2, n);
		result[size - 1] = '\0';
	}
	return (result);
}

off_t	file_length(char *file1)
{
	struct stat st;
	int			stat_return;

	stat_return = stat(file1, &st);
	if (stat_return == -1)
		perror("Error calling stat: ");
	return (st.st_size);
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
		return (1);
	fd2 = open(file2, O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd2 == -1)
		return (close(fd1), 1);
	bytes_read = read(fd1, buffer, sizeof(buffer));
	while (bytes_read == sizeof(buffer))
	{
		write_return = write(fd2, buffer, bytes_read);
		if (write_return == -1)
			return (close(fd1), close(fd2), 1);
		bytes_read = read(fd1, buffer, sizeof(buffer));
	}
	if (bytes_read == -1)
		return (close(fd1), close(fd2), 1);
	if (bytes_read > 0)
	{
		write_return = write(fd2, buffer, bytes_read);
		if (write_return == -1)
			return (close(fd1), close(fd2), 1);
	}
	return (0);
}
