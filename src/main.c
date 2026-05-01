#include "enigma.h"
#include "ui.h"
#ifdef HAS_GUI
#include "gui.h"
#endif
#include "preset.h"
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char* progname) {
    printf("Usage: %s [options]\n", progname);
    printf("Options:\n");
    printf("  -h, --help               Show this help message\n");
    printf("  -e, --encrypt <text>     Encrypt/decrypt the provided text\n");
    printf("  -p, --preset <name>      Load machine settings from a preset\n");
    printf("  -t, --test               Run internal logic tests\n");
    printf("  -g, --gui                Launch graphical user interface (default)\n");
    printf("  --tui                    Launch terminal user interface\n");
    printf("\nIf no options are provided, the interactive GUI will launch.\n");
}

void run_tests() {
    EnigmaMachine machine = createEnigmaMachine();
    const char* names[NUM_ROTORS] = {"I", "II", "III"};
    setRotorOrder(&machine, names);

    int ringSettings[3] = {1, 1, 1};
    setRingSettings(&machine, ringSettings);

    setReflector(&machine, "UKW-B");

    printf("\nDouble-stepping test:\n");
    int pos1[3] = {0, 3, 21}; // A D V
    setStartPositions(&machine, pos1);
    resetMachine(&machine);
    printf("Initial: %s\n", getRotorPositions(&machine));
    
    encryptChar(&machine, 'A');
    printf("Step 1 (ADV -> AEW): %s\n", getRotorPositions(&machine));
    
    encryptChar(&machine, 'A');
    printf("Step 2 (AEW -> BFX): %s (Double step!)\n", getRotorPositions(&machine));

    printf("\nPreset System test:\n");
    resetMachine(&machine);
    addPlugPairToMachine(&machine, 'A', 'Z');
    addPlugPairToMachine(&machine, 'B', 'Y');
    
    printf("Saving preset 'TestPreset'...\n");
    preset_save(&machine, "TestPreset");

    EnigmaMachine loadedMachine = createEnigmaMachine();
    if (preset_load(&loadedMachine, "TestPreset")) {
        printf("Preset loaded successfully.\n");
        char testStr[] = "ABC";
        char outStr[4];
        encryptString(&loadedMachine, testStr, outStr);
        printf("Encrypt 'ABC' with loaded machine: %s\n", outStr);
        
        resetMachine(&loadedMachine);
        char outStr2[4];
        encryptString(&loadedMachine, outStr, outStr2);
        printf("Roundtrip: %s -> %s\n", outStr, outStr2);
    }

    // Historical Test Vector
    printf("\nHistorical Verification (I, II, III | UKW-B | AAA):\n");
    EnigmaMachine hist;
    hist = createEnigmaMachine();
    const char* histRotors[3] = {"I", "II", "III"};
    setRotorOrder(&hist, histRotors);
    setReflector(&hist, "UKW-B");
    int histStarts[3] = {0, 0, 0};
    setStartPositions(&hist, histStarts);
    resetMachine(&hist);

    char histInput[] = "AAAAA";
    char histOutput[6];
    encryptString(&hist, histInput, histOutput);
    printf("Input:  %s\n", histInput);
    printf("Output: %s (Expected: BDZGO)\n", histOutput);
    
    if (strcmp(histOutput, "BDZGO") == 0) {
        printf("HISTORICAL VERIFICATION PASSED!\n");
    } else {
        printf("HISTORICAL VERIFICATION FAILED!\n");
    }
}

int main(int argc, char* argv[]) {
    int opt;
    char* encrypt_text = NULL;
    char* preset_name = NULL;
    int run_t = 0;
    int force_gui = 0;
    int force_tui = 0;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"encrypt", required_argument, 0, 'e'},
        {"preset", required_argument, 0, 'p'},
        {"test", no_argument, 0, 't'},
        {"gui", no_argument, 0, 'g'},
        {"tui", no_argument, 0, 1000}, // No short opt for TUI
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "he:p:t", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                return 0;
            case 'e':
                encrypt_text = optarg;
                break;
            case 'p':
                preset_name = optarg;
                break;
            case 't':
                run_t = 1;
                break;
            case 'g':
                force_gui = 1;
                break;
            case 1000:
                force_tui = 1;
                break;
            default:
                print_usage(argv[0]);
                return 1;
        }
    }

    if (run_t) {
        run_tests();
        return 0;
    }

    EnigmaMachine machine = createEnigmaMachine();
    
    // Set some defaults
    const char* defaultNames[NUM_ROTORS] = {"I", "II", "III"};
    setRotorOrder(&machine, defaultNames);
    setReflector(&machine, "UKW-B");

    // Load preset if requested
    if (preset_name) {
        if (!preset_load(&machine, preset_name)) {
            fprintf(stderr, "Error: Could not load preset '%s'\n", preset_name);
            return 1;
        }
    }

    if (encrypt_text) {
        // Headless mode
        char* output = malloc(strlen(encrypt_text) + 1);
        encryptString(&machine, encrypt_text, output);
        printf("%s\n", output);
        free(output);
    } else if (!isatty(STDIN_FILENO)) {
        // Pipe mode
        char buffer[1024];
        while (fgets(buffer, sizeof(buffer), stdin)) {
            size_t len = strlen(buffer);
            if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
            char* output = malloc(strlen(buffer) + 1);
            encryptString(&machine, buffer, output);
            printf("%s\n", output);
            free(output);
        }
    } else if (force_tui) {
        // Explicit TUI mode
        ui_init();
        ui_run(&machine);
        ui_cleanup();
    } else if (force_gui || isatty(STDIN_FILENO)) {
        // GUI mode (Default for TTY or explicit)
#ifdef HAS_GUI
        gui_init();
        gui_run(&machine);
        gui_cleanup();
#else
        if (force_gui) {
            fprintf(stderr, "Error: GUI support not compiled into this binary.\n");
            return 1;
        }
        // Fallback to TUI if no GUI is available
        ui_init();
        ui_run(&machine);
        ui_cleanup();
#endif
    } else {
        // Fallback for non-TTY interactive (though rare)
#ifdef HAS_GUI
        gui_init();
        gui_run(&machine);
        gui_cleanup();
#else
        ui_init();
        ui_run(&machine);
        ui_cleanup();
#endif
    }

    return 0;
}
