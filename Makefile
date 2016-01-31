all:
	gcc -c src/queueFIFO.c -o queue.o
	gcc src/main.c -o bin/pacman queue.o -lncurses -lpthread

clean:
	rm -f bin/pacman queue.o

run:
	./bin/pacman
