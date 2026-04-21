#ifndef PRESET_H
#define PRESET_H

#include "enigma.h"

typedef struct {
    char name[64];
    char rotorNames[NUM_ROTORS][8];
    int ringSettings[NUM_ROTORS];
    int startPositions[NUM_ROTORS];
    char reflector[16];
    char plugboard[MAX_PLUG_PAIRS][2];
    int plugboardCount;
} MachinePreset;

// Save current machine state as a preset
bool preset_save(const EnigmaMachine* machine, const char* presetName);

// Load machine state from a preset
bool preset_load(EnigmaMachine* machine, const char* presetName);

// List available presets (returns number of presets, names filled in buffer)
int preset_list(char names[][64], int maxPresets);

// Delete a preset
bool preset_delete(const char* presetName);

#endif // PRESET_H
