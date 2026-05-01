CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LIBS = -lncurses

# GUI Support Check
HAS_RAYLIB := $(shell pkg-config --exists raylib && echo 1 || echo 0)
ifeq ($(HAS_RAYLIB),1)
    RAYLIB_LIBS = $(shell pkg-config --libs raylib)
    RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
    GUI_SRC = src/gui.c
    CFLAGS += -DHAS_GUI
else
    # Only include GUI if explicitly requested or if headers might be in non-standard paths
    # For now, let's make it optional to allow testing core logic
    RAYLIB_LIBS = 
    GUI_SRC = 
endif

SRC = src/main.c src/enigma.c src/rotor.c src/reflector.c src/plugboard.c src/preset.c src/cJSON.c src/ui.c $(GUI_SRC)
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

