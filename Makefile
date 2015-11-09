all:
	gcc src/main.c -o bin/pacman

clean:
	rm -f bin/pacman
