

server:
	cc -g main.c sql.c tools.c server.c -o afsphere -lpq -lmicrohttpd

clean:
	rm -f afsphere
