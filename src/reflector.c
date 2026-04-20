#include "reflector.h"
#include <string.h>

// Helper functions (reuse from rotor if possible, but inline for simplicity)
static int letterToPosition(char letter) {
    if (letter >= 'A' && letter <= 'Z') {
        return letter - 'A';
    }
    return -1;
}

static char positionToLetter(int pos) {
    pos = (pos % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE;
    return 'A' + pos;
}

// Create reflector
Reflector createReflector(const char* wiring, const char* name) {
    Reflector rf;
    strncpy(rf.wiring, wiring, ALPHABET_SIZE);
    strncpy(rf.name, name, 8);
    rf.name[7] = '\0'; // safety
    return rf;
}

// Reflect signal: wiring[signal]
int reflectSignal(const Reflector* reflector, int signal) {
    return letterToPosition(reflector->wiring[signal]);
}

