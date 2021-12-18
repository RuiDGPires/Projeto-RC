CC := gcc
FLAGS = 

all: client

C_FILES := $(wildcard *.c)

debug: FLAGS = -D DEBUG
debug: all

client: $(C_FILES)
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm client 
