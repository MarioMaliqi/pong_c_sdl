all:
	gcc -g main.c -o pong $(shell pkg-config --cflags --libs sdl2) -lSDL2_ttf -lm
