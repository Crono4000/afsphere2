

server:
	cc -g main.c sql.c tools.c server.c render_pages.c -o afsphere -lpq -lmicrohttpd

server_bytea_limit_low:
	cc -g main.c sql.c tools.c server.c render_pages.c -o afsphere -lpq -lmicrohttpd -D BYTEA_LIMIT=2001

clean:
	rm -f afsphere
