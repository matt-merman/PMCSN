CC				= gcc
CFLAGS			= -Wall -Wextra -Ofast -g
LIB 			= -lm
SRC 			= ./src
LIBS 			= libs/rngs.c libs/rvgs.c libs/rvms.c
MAIN			= src/start.c
SOURCES			= src/init.c src/server.c src/events.c src/validation.c src/analytic.c utils/helpers.c src/event_list.c utils/files.c src/simulation.c
TEST_SOURCES	= test/analytic_test.c test/test.c test/events_test.c

all: compile compile_test compile_msq compile_estimate

compile:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) src/start.c  $(LIB) -o start

compile_test:
	$(CC) $(CFLAGS) $(LIBS) $(SOURCES) $(TEST_SOURCES) $(LIB) -o tests

compile_msq:
	$(CC) $(CFLAGS) libs/rngs.c libs/rvgs.c prog/msq.c libs/rvms.c src/analytic.c -o msq -lm

compile_estimate:
	$(CC) $(CFLAGS) libs/rngs.c libs/rvgs.c libs/rvms.c prog/estimate.c -o estimate -lm

finite_simulation: compile
	./start -s finite

infinite_simulation: compile
	./start -s infinite

test: compile_test
	./tests

msq: compile_msq
	./msq

estimate: compile_estimate
	./estimate < ./result/Primi.txt