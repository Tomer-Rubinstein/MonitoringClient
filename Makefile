client: client.c
	gcc -Wall -pedantic -ansi client.c -o client

clean:
	rm client
