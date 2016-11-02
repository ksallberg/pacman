build:
	gcc -c src/queueFIFO.c -o queue.o
	gcc src/main.c -o bin/pacman queue.o -lncurses -lpthread
	rm queue.o

clean:
	rm -f bin/pacman

run:
	./bin/pacman
