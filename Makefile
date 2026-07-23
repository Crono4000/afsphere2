

server:
	cc -g main.c file.c sql.c -o afsphere -lpq

clean:
	rm -f afsphere
