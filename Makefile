all:
	gcc -Wall -Werror -O3 -std=c99 hashtable.c test.c -o test.o

clean:
	rm *.o
