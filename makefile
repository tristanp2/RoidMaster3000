CC=g++
CFLAGS=-Wall

all:	

osx: 
	$(CC) -o game -Wall -g  *.cpp -framework SDL2 
linux:
	$(CC) -o game -Wall -g  *.cpp `sdl2-config --cflags --libs` 
