CC				= gcc
CFLAGS			= -Wall -Wextra
LIB 			= -lm
SRC 			= ./src
LIBS 			= libs/rngs.c libs/rvgs.c
MAIN			= src/start.c
SOURCES			= src/init.c src/server.c src/events.c src/validation.c src/analytic.c utils/helpers.c src/event_list.c
TEST_SOURCES	= test/analytic_test.c test/test.c test/events_test.c

all: compile compile_test compile_msq

compile:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) src/start.c  $(LIB) -o start -g

compile_test:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) $(TEST_SOURCES) $(LIB) -o tests -g

compile_msq:
	$(CC) $(CFLAGS) libs/rngs.c libs/rvgs.c prog/msq.c src/analytic.c -o msq -g -lm

run: compile
	./start

test: compile_test
	./tests

msq: compile_msq
	./msq