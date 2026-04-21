#include "enigma.h"
#include "ui.h"
#include "preset.h"
#include <stdio.h>
#include <string.h>

void run_tests() {
    EnigmaMachine machine = createEnigmaMachine();
    const char* names[NUM_ROTORS] = {"I", "II", "III"};
    setRotorOrder(&machine, names);

    int ringSettings[3] = {1, 1, 1};
    setRingSettings(&machine, ringSettings);

    setReflector(&machine, "UKW-B");

    // Test double-stepping: ADV -> AEW -> BFX
    printf("\nDouble-stepping test:\n");
    int pos1[3] = {0, 3, 21}; // A D V
    setStartPositions(&machine, pos1);
    resetMachine(&machine);
    printf("Initial: %s\n", getRotorPositions(&machine));
    
    encryptChar(&machine, 'A');
    printf("Step 1 (ADV -> AEW): %s\n", getRotorPositions(&machine));
    
    encryptChar(&machine, 'A');
    printf("Step 2 (AEW -> BFX): %s (Double step!)\n", getRotorPositions(&machine));

    // Test Phase 2: Preset System
    printf("\nPreset System test:\n");
    resetMachine(&machine);
    addPlugPairToMachine(&machine, 'A', 'Z');
    addPlugPairToMachine(&machine, 'B', 'Y');
    
    printf("Saving preset 'TestPreset'...\n");
    if (preset_save(&machine, "TestPreset")) {
        printf("Preset saved successfully.\n");
    }

    EnigmaMachine loadedMachine = createEnigmaMachine();
    printf("Loading preset 'TestPreset'...\n");
    if (preset_load(&loadedMachine, "TestPreset")) {
        printf("Preset loaded successfully.\n");
        printf("Rotor positions: %s\n", getRotorPositions(&loadedMachine));
        
        char testStr[] = "ABC";
        char outStr[4];
        encryptString(&loadedMachine, testStr, outStr);
        printf("Encrypt 'ABC' with loaded machine: %s\n", outStr);
        
        resetMachine(&loadedMachine);
        char outStr2[4];
        encryptString(&loadedMachine, outStr, outStr2);
        printf("Roundtrip: %s -> %s\n", outStr, outStr2);
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1 && strcmp(argv[1], "--test") == 0) {
        run_tests();
        return 0;
    }

    // Default: Launch TUI
    EnigmaMachine machine = createEnigmaMachine();
    const char* names[NUM_ROTORS] = {"I", "II", "III"};
    setRotorOrder(&machine, names);
    setReflector(&machine, "UKW-B");

    ui_init();
    ui_run(&machine);
    ui_cleanup();

    return 0;
}
