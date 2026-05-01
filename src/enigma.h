#ifndef ENIGMA_H
#define ENIGMA_H

#include "rotor.h"
#include "reflector.h"
#include "plugboard.h"
#include <stdio.h> // for char*
#include <stdbool.h>

#define NUM_ROTORS 3
#define MAX_PLUG_PAIRS 10

typedef struct {
    Rotor rotors[NUM_ROTORS];
    Reflector reflector;
    Plugboard plugboard;
    const char* rotorNames[NUM_ROTORS]; // e.g., "I", "II", "III"
    int ringSettings[NUM_ROTORS];
    int startPositions[NUM_ROTORS];
} EnigmaMachine;

// Create default Enigma machine (I-II-III, UKW-B, no plugs)
EnigmaMachine createEnigmaMachine(void);

// Set rotor order (e.g., {"I", "II", "III"})
void setRotorOrder(EnigmaMachine* machine, const char* rotorNames[NUM_ROTORS]);

// Set ring settings (1-26 for each rotor)
void setRingSettings(EnigmaMachine* machine, const int settings[NUM_ROTORS]);

// Set start positions (0-25 for each rotor)
void setStartPositions(EnigmaMachine* machine, const int positions[NUM_ROTORS]);

// Set reflector (e.g., "UKW-B")
void setReflector(EnigmaMachine* machine, const char* reflectorName);

// Add plugboard pair (up to 10)
bool addPlugPairToMachine(EnigmaMachine* machine, char from, char to);

// Remove plugboard pair
void removePlugPairFromMachine(EnigmaMachine* machine, char c);

// Encrypt single character ('A'-'Z')
char encryptChar(EnigmaMachine* machine, char input);

// Encrypt string (in-place or to buffer)
void encryptString(EnigmaMachine* machine, const char* input, char* output);

// Step all rotors (with double-stepping)
void stepRotors(EnigmaMachine* machine);

// Reset to start positions
void resetMachine(EnigmaMachine* machine);

// Get current rotor positions as string (e.g., "AAZ")
char* getRotorPositions(const EnigmaMachine* machine);

#endif // ENIGMA_H

