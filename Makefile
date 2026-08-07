

server:
	cc -g main.c file.c sql.c tools.c -o afsphere -lpq -lmicrohttpd

clean:
	rm -f afsphere
