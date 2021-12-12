CC := gcc
FLAGS = 

all: udpclient

debug: FLAGS = -D DEBUG
debug: all

udpclient: udpclient.c
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm udpclient 
