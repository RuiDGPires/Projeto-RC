CC := gcc
FLAGS = 

all: udpclient

C_FILES := $(wildcard *.c)

debug: FLAGS = -D DEBUG
debug: all

udpclient: *.c
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm udpclient 
