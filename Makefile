all:
	gcc src/main.c -o bin/pacman -lncurses -lpthread

clean:
	rm -f bin/pacman

run:
	./bin/pacman
