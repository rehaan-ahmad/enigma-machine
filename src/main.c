#include "enigma.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        (void)argv;
        printf("Usage: ./enigma --test\n");
        return 1;
    }

    EnigmaMachine machine = createEnigmaMachine();
    const char* names[NUM_ROTORS] = {"I", "II", "III"};
    setRotorOrder(&machine, names);

    int ringSettings[3] = {1, 1, 1};
    setRingSettings(&machine, ringSettings);

    int startPositions[3] = {0, 0, 25}; // A A Z
    setStartPositions(&machine, startPositions);

    setReflector(&machine, "UKW-B");

    // Test double-stepping: ADV -> AEW -> BFX
    // Rotor III notch is V, Rotor II notch is E
    printf("\nDouble-stepping test:\n");
    resetMachine(&machine);
    int pos1[3] = {0, 3, 21}; // A D V
    setStartPositions(&machine, pos1);
    printf("Initial: %s\n", getRotorPositions(&machine));
    
    encryptChar(&machine, 'A');
    printf("Step 1 (ADV -> AEW): %s\n", getRotorPositions(&machine));
    
    encryptChar(&machine, 'A');
    printf("Step 2 (AEW -> BFX): %s (Double step!)\n", getRotorPositions(&machine));

    return 0;
}
