CC=gcc
FLAGS=-c

mancala: lib
	$(CC) -I . -L . mancala.c -l mancala -o mancala

lib: libmancala.o
	ar rcs libmancala.a libmancala.o

libmancala.o:
	$(CC) $(FLAGS) libmancala.c

clean:
	rm *.a *.o mancala
