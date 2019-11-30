# Author Alex Vigneron
CC = gcc	#compiler
CFLAGS = -W -Wall -Wextra -std=c99 -pedantic -g
TARGET = path_in_maze

%.o : %.c	#general receipe to obtain a .o from a .c
	$(CC) -c $(CFLAGS) $^

all: $(TARGET)

path_in_maze: path_in_maze.o graphe.o
	$(CC) -o $@ $^

.PHONY: all clean

clean:
	rm -rf *.o *.ps a.out *.dot

mrproper:
	make clean
	rm $(TARGET)
