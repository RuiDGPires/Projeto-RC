CC := gcc
FLAGS = 

all: user

C_FILES := $(wildcard *.c) $(wildcard ../common/*.c)

debug: FLAGS = -D DEBUG
debug: all

user: $(C_FILES)
	$(CC) $(FLAGS) -Wall -Werror -o $@ $^

clean:
	rm user 
