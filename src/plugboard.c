#include "plugboard.h"
#include <stdbool.h>
#include <string.h>

static int letterToPosition(char letter) {
    if (letter >= 'A' && letter <= 'Z') return letter - 'A';
    return -1;
}

// Create empty (identity) plugboard
Plugboard createPlugboard(void) {
    Plugboard pb;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        pb.map[i] = i;
    }
    pb.pairsCount = 0;
    return pb;
}

// Add pair, validate unique letters, max pairs
bool addPlugPair(Plugboard* pb, char fromLetter, char toLetter) {
    int from = letterToPosition(fromLetter);
    int to = letterToPosition(toLetter);
    if (from < 0 || to < 0 || from == to || pb->map[from] != from || pb->map[to] != to || pb->pairsCount >= MAX_PLUG_PAIRS) {
        return false;
    }
    pb->map[from] = to;
    pb->map[to] = from;
    pb->pairsCount++;
    return true;
}

// Clear
void clearPlugboard(Plugboard* pb) {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        pb->map[i] = i;
    }
    pb->pairsCount = 0;
}

// Swap
int plugboardSwap(const Plugboard* pb, int signal) {
    return pb->map[signal];
}

