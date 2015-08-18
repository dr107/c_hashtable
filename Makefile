CC=gcc
OPTS=-Wall -Werror -O3 -std=c99

SRC=hashtable.c
SRC_O=hashtable.o
SO=hashtable.so

TEST=test.c
TEST_O=test.o

all:
	$(CC) $(OPTS) $(SRC) $(TEST) -o $(TEST_O)
