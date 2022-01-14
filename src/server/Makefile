CC := gcc
FLAGS = 

all: DS

C_FILES := $(wildcard *.c) $(wildcard ../common/*.c)

debug: FLAGS = -D DEBUG
debug: all

DS: $(C_FILES)
	$(CC) $(FLAGS) -Wall -Werror -o $@ $^

clean:
	rm DS
