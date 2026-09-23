
#ifndef AFSPHERE_H
# define AFSPHERE_H

# include <stdio.h>
# include <sys/stat.h>
# include <postgresql/libpq-fe.h>
# include <postgresql/libpq/libpq-fs.h>
# include <microhttpd.h>
# include <string.h>
# include <stdarg.h>
# include <unistd.h>
# include <signal.h>
# include <fcntl.h>
# include <stdlib.h>
# include <signal.h>
# include <sys/wait.h>

# define CPY_BUFFER 50
# define BYTEA_LIMIT 5000000

# define FLAG_ISBYTEA 1
# define FLAG_ISFILE 2

typedef struct info_s
{
	char			*file_id;
	char			*title;
	unsigned char	*bytea;
	Oid				oid;
	long			size;
	short			flags;
}					info_t;

typedef struct app_s
{
	PGconn				*conn;
	struct MHD_Daemon	*daemon;
}						app_t;

int		render_page_sql(PGconn *conn, int fd, char *file, char *template, char *query, char **args, int size);

int		get_file_length(char *file1, off_t *length);
int		copy_file(char *file1, char *file2);
char	*ltoa(unsigned long	n);
char	*get_file_name(char	*path);
int		get_file_content(char **content, char *file);
int		split_join(char **split, int size, char between, char final1, char final2, char **result);
char	**str_array(int size, ...);

int		search_info_db(PGconn *conn, char **tags, int size);
int		add_file_db(PGconn *conn, char* file);
int		exec_file_db(PGconn *conn, char* file);
int		show_query_db(PGconn *conn, char *query);
int		add_info_bytea_db(PGconn *conn, char *title, void* content, long size, char *is_file, char **file_id);
int		format_db(PGconn *conn);
int		add_content_db(PGconn *conn, char *title, char* content, char **tags, int size);
int 	add_info_tags_db(PGconn *conn, char *info_id, char **tags, int size);
int		add_info_tag_db(PGconn *conn, char *info_id, char *tag);
int		show_info_db(PGconn *conn, char *file_id);
int		put_info_into_file_db(PGconn *conn, char *file_id);

int		stop_server();
int		start_server(PGconn	*conn);

#endif
