all: control.c client.c
	gcc -o host control.c
	gcc -o client client.c

clean:
	rm -rf *~
	./host -r
	rm host
	rm client
