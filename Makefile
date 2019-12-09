# Author Alex Vigneron

CC = gcc	#compiler
CFLAGS = -W -Wall -Wextra -std=c99 -pedantic -g -Wno-unused-parameter
TARGET = path_in_maze naive_dijkstra max_valued_path

%.o : %.c	#general receipe to obtain a .o from a .c
	$(CC) -c $(CFLAGS) $^

all: $(TARGET)

path_in_maze: DFS_path_in_maze.o graphe.o
	$(CC) -o $@ $^

naive_dijkstra: naive_dijkstra.o graphe.o
	$(CC) -o $@ $^

max_valued_path: max_valued_path.o graphe.o
	$(CC) -o $@ $^


.PHONY: all clean

clean:
	rm -rf *.o *.ps a.out *.dot

mrproper:
	make clean
	rm $(TARGET)
