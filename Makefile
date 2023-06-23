CC := gcc
#CFLAGS := 
DEST := ./build/

all: tideman.c
	mkdir -p build
	$(CC) tideman.c -lcs50 -o $(DEST)/tideman 