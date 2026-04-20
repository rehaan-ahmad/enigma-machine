#include "rotor.h"
#include <string.h>
#include <stdlib.h>

// Create a rotor with the specified wiring and notch
Rotor rotorCreate(const char* wiring, char notch, char name) {
    Rotor rotor;
    strncpy(rotor.wiring, wiring, ALPHABET_SIZE);
    rotor.notch = notch;
    rotor.ringSetting = 1;
    rotor.currentPosition = 0;
    rotor.name = name;
    return rotor;
}

// Convert a letter to its position (0-25)
static int letterToPosition(char letter) {
    if (letter >= 'A' && letter <= 'Z') {
        return letter - 'A';
    }
    return -1;
}

// Convert a position (0-25) to a letter (A-Z)
static char positionToLetter(int position) {
    position = ((position % ALPHABET_SIZE) + ALPHABET_SIZE) % ALPHABET_SIZE;
    return 'A' + position;
}

// Forward pass through the rotor (right to left)
int rotorForward(const Rotor* rotor, int signal) {
    // Apply ring setting
    int adjustedSignal = (signal + rotor->currentPosition - rotor->ringSetting + 1 + ALPHABET_SIZE) % ALPHABET_SIZE;

    // Get the output from the wiring
    char outputLetter = rotor->wiring[adjustedSignal];
    int outputSignal = letterToPosition(outputLetter);

    // Reverse the ring setting
    int finalSignal = (outputSignal - rotor->currentPosition + rotor->ringSetting - 1 + ALPHABET_SIZE) % ALPHABET_SIZE;

    return finalSignal;
}

// Backward pass through the rotor (left to right)
int rotorBackward(const Rotor* rotor, int signal) {
    // Apply ring setting
    int adjustedSignal = (signal + rotor->currentPosition - rotor->ringSetting + 1 + ALPHABET_SIZE) % ALPHABET_SIZE;

    // Find the input position in the wiring that gives the adjusted signal
    char targetLetter = positionToLetter(adjustedSignal);
    int inputPosition = -1;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (rotor->wiring[i] == targetLetter) {
            inputPosition = i;
            break;
        }
    }

    // Reverse the ring setting
    int finalSignal = (inputPosition - rotor->currentPosition + rotor->ringSetting - 1 + ALPHABET_SIZE) % ALPHABET_SIZE;

    return finalSignal;
}

// Step the rotor forward by one position
void rotorStep(Rotor* rotor) {
 rotor->currentPosition = (rotor->currentPosition + 1) % ALPHABET_SIZE;
}

// Set the rotor position (0-25)
void rotorSetPosition(Rotor* rotor, int position) {
rotor->currentPosition = ((position % ALPHABET_SIZE) + ALPHABET_SIZE) % ALPHABET_SIZE;
}

// Set the ring setting (1-26)
void rotorSetRingSetting(Rotor* rotor, int ringSetting) {
    rotor->ringSetting = ((ringSetting - 1) % ALPHABET_SIZE) + 1;
}

// Get the current rotor position as a letter (A-Z)
char rotorGetPositionLetter(const Rotor* rotor) {
    return positionToLetter(rotor->currentPosition);
}

// Check if the rotor is at its notch position
bool rotorIsAtNotch(const Rotor* rotor) {
    return rotor->currentPosition == letterToPosition(rotor->notch);
}

// Reset rotor to initial state
void rotorReset(Rotor* rotor) {
    rotor->currentPosition = 0;
    rotor->ringSetting = 1;
}
