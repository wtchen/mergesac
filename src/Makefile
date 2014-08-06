CC=gcc
CFLAGS=-c -Wall
HEADS=sachead64.h

all: build

build: mergesac.o
	$(CC) mergesac.o -o mergesac

mergesac.o: mergesac.c
	$(CC) $(CFLAGS) mergesac.c $(HEADS)

.PHONY: clean
clean:
	rm -f *.o *.gch mergesac

.PHONY: install
install:
	cp mergesac /usr/local/bin
