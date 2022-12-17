CC				= gcc
CFLAGS			= -Wall -Wextra
LIB 			= -lm
SRC 			= ./src
LIBS 			= ./libs
UTILS 			= ./utils

all:
	$(CC) $(CFLAGS) libs/rngs.c libs/rvgs.c src/server.c src/init.c src/start.c src/events.c src/validation.c utils/helpers.c $(LIB) -o start -g

run: all
	./start

test:
	$(CC) $(CFLAGS) libs/rngs.c libs/rvgs.c src/server.c src/start.c src/events.c utils/helpers.c $(LIB) -o tests -g
	./tests