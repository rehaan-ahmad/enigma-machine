CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LIBS = -lncurses

# Raylib flags (Linux default)
RAYLIB_LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# OS detection
ifeq ($(OS),Windows_NT)
    RAYLIB_LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        RAYLIB_LIBS = -lraylib -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
    endif
endif

SRC = src/main.c src/enigma.c src/rotor.c src/reflector.c src/plugboard.c src/preset.c src/cJSON.c src/ui.c src/gui.c
OBJ = $(SRC:.c=.o)

enigma: $(OBJ)
	$(CC) $(OBJ) -o enigma $(LIBS) $(RAYLIB_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) enigma

test: enigma
	./enigma --test

.PHONY: clean test

