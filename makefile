CC=g++
CFLAGS=-Wall

all:    comp

comp: 
	$(CC) -o game -Wall -g  *.cpp -framework SDL2 

