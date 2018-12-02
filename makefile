CC=gcc -g -Wall
CFLAGS=-I.
DEPS = function.h
OBJ = main.o function.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
proj4: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
    rm -f *.o proj4