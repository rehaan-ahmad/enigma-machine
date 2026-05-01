#ifndef ROTOR_PRESETS_H
#define ROTOR_PRESETS_H

#include "rotor.h"

typedef struct {
    const char* name;
    char wiring[ALPHABET_SIZE + 1];
    char notch;
} RotorPreset;

static const RotorPreset rotorPresets[] = {
    {"I", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", 'Q'},
    {"II", "AJDKSIRUXBLHWTMCQGZNPYFVOE", 'E'},
    {"III", "BDFHJLCPRTXVZNYEIWGAKMUSQO", 'V'},
    {"IV", "ESOVPZJAYQUIRHXLNFTGKDCMWB", 'J'},
    {"V", "VZBRGITYUPSDNHLXAWMJQOFECK", 'Z'},
};

static const int numRotorPresets = sizeof(rotorPresets) / sizeof(RotorPreset);

#endif
