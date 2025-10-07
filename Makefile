CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99
SRC = src/ls-v1.0.0.c
BIN = bin/ls-v1.0.0

.PHONY: all clean

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) $(SRC) -o $(BIN)

clean:
	rm -rf bi
