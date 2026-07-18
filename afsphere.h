
#ifndef AFSPHERE_H
# define AFSPHERE_H

# include <stdio.h>
# include <sys/stat.h>
# include <postgresql/libpq-fe.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>

# define CPY_BUFFER 50

off_t	file_length(char *file1);
int		copy_file(char *file1, char *file2);
char	*ltoa(unsigned long	n);

#endif
