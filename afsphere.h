
#ifndef AFSPHERE_H
# define AFSPHERE_H

# include <stdio.h>
# include <sys/stat.h>
# include <postgresql/libpq-fe.h>
# include <microhttpd.h>
# include <string.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <signal.h>
# include <sys/wait.h>

typedef struct apc_config_s
{
	char	*user_name;
}			apc_config_t

int		get_file_length(char *file1, off_t *length);
int		copy_file(char *file1, char *file2);
char	*ltoa(unsigned long	n);
char	*get_file_name(char	*path);
int		get_file_content(char **content, char *file);

int		add_disk_db(PGconn *conn, char* disk, char	*limit, char *priority);
int		move_file_to_db(PGconn *conn, char* file, char* file_id);
int		add_file_db(PGconn *conn, char* file);
int		exec_file_db(PGconn *conn, char* file);
int		show_query_db(PGconn *conn, char *query);

#endif
