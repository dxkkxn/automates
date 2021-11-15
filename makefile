CC = gcc
CFLAGS = -Wall -g
OBJ = mygrep.o regcomp.o parser.o scanner.o stack.o afn.o afd.o

mygrep: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o mygrep -lm

mygrep.o: mygrep.c afd.h scanner.h parser.h regcomp.h
	$(CC) $(CFLAGS) -c mygrep.c

regcomp.o: regcomp.c regcomp.h scanner.h stack.h afn.h afd.h
	$(CC) $(CFLAGS) -c regcomp.c

parser.o: parser.c parser.h scanner.h
	$(CC) $(CFLAGS) -c parser.c

scanner.o: scanner.c  scanner.h
	$(CC) $(CFLAGS) -c scanner.c

stack.o: stack.c stack.h afn.h
	$(CC) $(CFLAGS) -c stack.c

afn.o: afn.c afn.h afd.h
	$(CC) $(CFLAGS) -c afn.c -lm

afd.o: afd.c afd.h
	$(CC) $(CFLAGS) -c afd.c

clean:
	rm -f *.o *~
