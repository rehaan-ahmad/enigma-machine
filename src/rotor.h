#ifndef ROTOR_H
#define ROTOR_H

#include <stdbool.h>

#define ALPHABET_SIZE 26

typedef struct {
    char wiring[ALPHABET_SIZE + 1];  
    char notch;                  
    int ringSetting;             
    int currentPosition;         
    char name;                   
} Rotor;

// Create a rotor with the specified wiring and notch
Rotor rotorCreate(const char* wiring, char notch, char name);

// Forward pass through the rotor (right to left)
int rotorForward(const Rotor* rotor, int signal);

// Backward pass through the rotor (left to right)
int rotorBackward(const Rotor* rotor, int signal);

// Step the rotor forward by one position
void rotorStep(Rotor* rotor);

// Set the rotor position (0-25)
void rotorSetPosition(Rotor* rotor, int position);

// Set the ring setting (1-26)
void rotorSetRingSetting(Rotor* rotor, int ringSetting);

// Get the current rotor position as a letter (A-Z)
char rotorGetPositionLetter(const Rotor* rotor);

// Check if the rotor is at its notch position
bool rotorIsAtNotch(const Rotor* rotor);

// Reset rotor to initial state
void rotorReset(Rotor* rotor);

#endif // ROTOR_H
