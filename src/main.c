#include "enigma.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./enigma --test\\n");
        return 1;
    }

    EnigmaMachine machine = createEnigmaMachine();
    char rotorNames[NUM_ROTORS][2] = {"I", "II", "III"};
    const char* names[3] = {rotorNames[0], rotorNames[1], rotorNames[2]};
    setRotorOrder(&machine, names);

    int ringSettings[3] = {1, 1, 1};
    setRingSettings(&machine, ringSettings);

    int startPositions[3] = {0, 0, 25}; // A A Z
    setStartPositions(&machine, startPositions);

    setReflector(&machine, "UKW-B");

    // Test encrypt "AAA"
    char input[] = "AAA";
    char output[4];
    encryptString(&machine, input, output);
    printf("Input: %s -> Output: %s Positions: %s\\n", input, output, getRotorPositions(&machine));

    // Test self-inverse
    char output2[4];
    resetMachine(&machine);
    encryptString(&machine, output, output2);
    printf("Roundtrip: %s -> %s\\n", output, output2);

    return 0;
}

