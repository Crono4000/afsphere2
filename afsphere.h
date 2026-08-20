
#ifndef AFSPHERE_H
# define AFSPHERE_H

# include <stdio.h>
# include <sys/stat.h>
# include <postgresql/libpq-fe.h>
# include <microhttpd.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <stdlib.h>
# include <signal.h>
# include <sys/wait.h>

# define CPY_BUFFER 50
# define BYTEA_LIMIT 2000000

typedef struct app_s
{
	PGconn				*conn;
	struct MHD_Daemon	*daemon;
}						app_t;

int		get_file_length(char *file1, off_t *length);
int		copy_file(char *file1, char *file2);
char	*ltoa(unsigned long	n);
char	*get_file_name(char	*path);
int		get_file_content(char **content, char *file);
int		split_join(char **split, int size, char between, char final1, char final2, char **result);

int		add_disk_db(PGconn *conn, char* disk, char	*limit, char *priority);
int		move_file_to_db(PGconn *conn, char* file, char* file_id);
int		add_file_db(PGconn *conn, char* file);
int		exec_file_db(PGconn *conn, char* file);
int		show_query_db(PGconn *conn, char *query);
int		add_info_bytea_db(PGconn *conn, char *title, void* content, long size, char *is_file, char **file_id);
int		format_db(PGconn *conn);
int		add_content_db(PGconn *conn, char *title, char* content, char **tags, int size);
int 	add_info_tags_db(PGconn *conn, char *info_id, char **tags, int size);
int		add_info_tag_db(PGconn *conn, char *info_id, char *tag);

int		stop_server();
int		start_server(PGconn	*conn);

#endif
