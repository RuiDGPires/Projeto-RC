
CC := gcc

SERVER_C_FILES := $(wildcard src/server/*.c) $(wildcard src/common/*.c)
USER_C_FILES := $(wildcard src/user/*.c) $(wildcard src/common/*.c)

all: DS user

DS: $(SERVER_C_FILES)
	$(CC) -o $@ $^
user: $(USER_C_FILES)
	$(CC) -o $@ $^

clean:
	rm DS user
