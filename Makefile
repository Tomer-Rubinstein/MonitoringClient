client: client.c postData.o getters.o stringOperations.o
	gcc -Wall -pedantic postData.o stringOperations.o getters.o client.c -o client

postData.o: postData.c
	gcc -c -Wall -pedantic postData.c -o postData.o

getters.o: getters.c
	gcc -c -Wall -pedantic getters.c -o getters.o

stringOperations.o: stringOperations.c
	gcc -c -Wall -pedantic stringOperations.c -o stringOperations.o

clean:
	rm *.o client
