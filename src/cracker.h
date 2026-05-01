#ifndef CRACKER_H
#define CRACKER_H

#include "enigma.h"

typedef struct {
    const char* ciphertext;
    const char* crib;
    // Results
    char found_rotor_names[3][8];
    int found_start_pos[3];
    bool success;
    float progress; // 0.0 to 1.0
} CrackerStatus;

// Run the cracking process (blocking, should be run in a thread or with progress updates)
void crack_code(CrackerStatus* status);

#endif
