# all: main.o main

main: main.c game.o game.h 
	gcc -I src/include -L src/lib -o main main.c game.o -lmingw32 -lSDL2main -lSDL2

game.o: game.c game.h
	gcc -g -c game.c

