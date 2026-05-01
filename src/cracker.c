#include "cracker.h"
#include "rotor_presets.h"
#include <string.h>
#include <stdlib.h>

void crack_code(CrackerStatus* status) {
    status->success = false;
    status->progress = 0;
    
    int total_combos = numRotorPresets * (numRotorPresets - 1) * (numRotorPresets - 2);
    int total_steps = total_combos * 26 * 26 * 26;
    int current_step = 0;

    EnigmaMachine test_machine = createEnigmaMachine();
    char* test_output = malloc(strlen(status->ciphertext) + 1);

    // Iterate through all rotor combinations
    for (int r1 = 0; r1 < numRotorPresets; r1++) {
        for (int r2 = 0; r2 < numRotorPresets; r2++) {
            if (r2 == r1) continue;
            for (int r3 = 0; r3 < numRotorPresets; r3++) {
                if (r3 == r1 || r3 == r2) continue;

                const char* names[3] = { rotorPresets[r1].name, rotorPresets[r2].name, rotorPresets[r3].name };
                setRotorOrder(&test_machine, names);

                // Iterate through all start positions
                for (int p1 = 0; p1 < 26; p1++) {
                    for (int p2 = 0; p2 < 26; p2++) {
                        for (int p3 = 0; p3 < 26; p3++) {
                            int pos[3] = { p1, p2, p3 };
                            setStartPositions(&test_machine, pos);
                            resetMachine(&test_machine);

                            encryptString(&test_machine, status->ciphertext, test_output);

                            if (strstr(test_output, status->crib) != NULL) {
                                // Found it!
                                strcpy(status->found_rotor_names[0], names[0]);
                                strcpy(status->found_rotor_names[1], names[1]);
                                strcpy(status->found_rotor_names[2], names[2]);
                                status->found_start_pos[0] = p1;
                                status->found_start_pos[1] = p2;
                                status->found_start_pos[2] = p3;
                                status->success = true;
                                status->progress = 1.0;
                                free(test_output);
                                return;
                            }

                            current_step++;
                            if (current_step % 10000 == 0) {
                                status->progress = (float)current_step / total_steps;
                            }
                        }
                    }
                }
            }
        }
    }

    free(test_output);
    status->progress = 1.0;
}
