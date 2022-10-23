all:
	gcc -Wall -Wextra libs/rngs.c libs/rvgs.c src/server.c src/start.c src/events.c utils/helpers.c -lm -o start -g

run: all
	./start

test:
	gcc -Wall -Wextra libs/rngs.c libs/rvgs.c src/server.c src/start.c src/events.c utils/helpers.c -lm -o tests -g
	./tests