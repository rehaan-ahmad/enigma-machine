#include "enigma.h"
#include "rotor.h"
#include "reflector.h"
#include "plugboard.h"
#include <string.h>
#include <stdio.h>

// Hardcoded rotor presets from historical data
typedef struct {
    char name;
    char wiring[ALPHABET_SIZE];
    char notch;
} RotorPreset;

static const RotorPreset rotorPresets[] = {
    {'I', "EKMFLGDQVZNTOWYHXUSPAIBRCJ", 'Q'},
    {'II', "AJDKSIRUXBLHWTMCQGZNPYFVOE", 'E'},
    {'III', "BDFHJLCPRTXVZNYEIWGAKMUSQO", 'V'},
    {'IV', "ESOVPZJAYQUIRHXLNFTGKDCMWB", 'J'},
    {'V', "VZBRGITYUPSDNHLXAWMJQOFECK", 'Z'},
    // Add VI-VIII later for Kriegsmarine
};

static const int numRotorPresets = sizeof(rotorPresets) / sizeof(RotorPreset);

static const char* reflectorWirings[] = {
    "YRUHQSLDPXNGOKMIEBFZCWVJAT", // UKW-B Thin? TODO check exact
    // Add UKW-A, UKW-C
};

EnigmaMachine createEnigmaMachine(void) {
    EnigmaMachine machine;
    for (int i = 0; i < NUM_ROTORS; i++) {
        machine.rotors[i] = rotorCreate("ABCDEFGHIJKLMNOPQRSTUVWXYZ", ' ', ' '); // dummy
    }
    machine.reflector = createReflector("YRUHQSLDPXNGOKMIEBFZCWVJAT", "UKW-B");
    machine.plugboard = createPlugboard();
    memset(machine.rotorOrder, 0, sizeof(machine.rotorOrder));
    memset(machine.ringSettings, 1, sizeof(machine.ringSettings));
    memset(machine.startPositions, 0, sizeof(machine.startPositions));
    return machine;
}

void setRotorOrder(EnigmaMachine* machine, const char* rotorNames[NUM_ROTORS]) {
    for (int i = 0; i < NUM_ROTORS; i++) {
        for (int j = 0; j < numRotorPresets; j++) {
            if (rotorPresets[j].name == rotorNames[i][0]) {
                machine->rotors[i] = rotorCreate(rotorPresets[j].wiring, rotorPresets[j].notch, rotorPresets[j].name);
                machine->rotorOrder[i] = rotorPresets[j].name;
                rotorSetRingSetting(&machine->rotors[i], machine->ringSettings[i]);
                rotorSetPosition(&machine->rotors[i], machine->startPositions[i]);
                break;
            }
        }
    }
}

void setRingSettings(EnigmaMachine* machine, const int settings[NUM_ROTORS]) {
    memcpy(machine->ringSettings, settings, sizeof(machine->ringSettings));
    for (int i = 0; i < NUM_ROTORS; i++) {
        rotorSetRingSetting(&machine->rotors[i], settings[i]);
    }
}

void setStartPositions(EnigmaMachine* machine, const int positions[NUM_ROTORS]) {
    memcpy(machine->startPositions, positions, sizeof(machine->startPositions));
    for (int i = 0; i < NUM_ROTORS; i++) {
        rotorSetPosition(&machine->rotors[i], positions[i]);
    }
}

void setReflector(EnigmaMachine* machine, const char* reflectorName) {
    if (strncmp(reflectorName, "UKW-B", 5) == 0) {
        machine->reflector = createReflector("YRUHQSLDPXNGOKMIEBFZCWVJAT", "UKW-B");
    }
    // Add more
}

bool addPlugPair(EnigmaMachine* machine, char from, char to) {
    return addPlugPair(&machine->plugboard, from, to); // from plugboard.c
}

char encryptChar(EnigmaMachine* machine, char input) {
    if (input < 'A' || input > 'Z') return input;

    int signal = input - 'A';

    // 1. Plugboard in
    signal = plugboardSwap(&machine->plugboard, signal);

    // 2. Rotors forward: right -> middle -> left
    signal = rotorForward(&machine->rotors[2], signal); // right
    signal = rotorForward(&machine->rotors[1], signal); // middle
    signal = rotorForward(&machine->rotors[0], signal); // left

    // 3. Reflect
    signal = reflectSignal(&machine->reflector, signal);

    // 4. Rotors backward: left -> middle -> right
    signal = rotorBackward(&machine->rotors[0], signal);
    signal = rotorBackward(&machine->rotors[1], signal);
    signal = rotorBackward(&machine->rotors[2], signal);

    // 5. Plugboard out
    signal = plugboardSwap(&machine->plugboard, signal);

    // 6. Step rotors (historical: after encryption)
    stepRotors(machine);

    return 'A' + signal;
}

void stepRotors(EnigmaMachine* machine) {
    // Right rotor always steps
    rotorStep(&machine->rotors[2]);

    // Double stepping check
    if (rotorIsAtNotch(&machine->rotors[2]) || rotorIsAtNotch(&machine->rotors[1])) {
        rotorStep(&machine->rotors[1]);
        if (rotorIsAtNotch(&machine->rotors[1])) {
            rotorStep(&machine->rotors[0]);
        }
    }
}

void resetMachine(EnigmaMachine* machine) {
    for (int i = 0; i < NUM_ROTORS; i++) {
        rotorSetPosition(&machine->rotors[i], machine->startPositions[i]);
    }
}

char* getRotorPositions(const EnigmaMachine* machine) {
    static char pos[4];
    for (int i = 0; i < NUM_ROTORS; i++) {
        pos[i] = rotorGetPositionLetter(&machine->rotors[i]);
    }
    pos[3] = '\0';
    return pos;
}

void encryptString(EnigmaMachine* machine, const char* input, char* output) {
    int len = strlen(input);
    for (int i = 0; i < len; i++) {
        output[i] = encryptChar(machine, input[i]);
    }
    output[len] = '\0';
}

