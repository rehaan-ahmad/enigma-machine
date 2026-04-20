CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LIBS = -lncurses

SRC = src/main.c src/enigma.c src/rotor.c src/reflector.c src/plugboard.c
OBJ = $(SRC:.c=.o)

enigma: $(OBJ)
	$(CC) $(OBJ) -o enigma $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) enigma

test: enigma
	./enigma --test

.PHONY: clean test

