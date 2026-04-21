#ifndef PLUGBOARD_H
#define PLUGBOARD_H

#include <stdbool.h>

#define ALPHABET_SIZE 26
#define MAX_PLUG_PAIRS 10

typedef struct {
    int map[ALPHABET_SIZE]; // map[i] = swapped position, or i if none
    int pairsCount;
} Plugboard;

// Create empty plugboard
Plugboard createPlugboard(void);

// Add plug pair (A-Z letters), bidirectional, returns false if invalid/max
bool addPlugPair(Plugboard* pb, char fromLetter, char toLetter);

// Clear all plugs
void clearPlugboard(Plugboard* pb);

// Swap signal through plugboard
int plugboardSwap(const Plugboard* pb, int signal);

#endif // PLUGBOARD_H

