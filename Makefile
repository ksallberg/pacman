build:
	gcc -c src/double_link.c -o list.o
	gcc src/main.c -o bin/pacman list.o -lncurses -lpthread
	rm list.o

clean:
	rm -f bin/pacman

run:
	./bin/pacman
