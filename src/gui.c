#include "gui.h"
#include <raylib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 700

// Colors
#define COLOR_BG (Color){ 24, 24, 24, 255 }
#define COLOR_PANEL (Color){ 40, 40, 40, 255 }
#define COLOR_ROTOR (Color){ 60, 60, 60, 255 }
#define COLOR_LAMP_OFF (Color){ 30, 30, 30, 255 }
#define COLOR_LAMP_ON (Color){ 255, 230, 100, 255 }
#define COLOR_KEY (Color){ 50, 50, 50, 255 }
#define COLOR_TEXT (Color){ 200, 200, 200, 255 }

static const char* KB_ROWS[] = {"QWERTZUIO", "ASDFGHJK", "PYXCVBNM"};
static int KB_OFFSETS[] = {0, 25, 50};

void gui_init(void) {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enigma Machine Simulator - Raylib GUI");
    SetTargetFPS(60);
}

void gui_cleanup(void) {
    CloseWindow();
}

static void draw_rotor(int x, int y, int pos, const char* name) {
    DrawRectangleRounded((Rectangle){ x - 40, y - 60, 80, 120 }, 0.2, 10, COLOR_ROTOR);
    DrawRectangleRoundedLines((Rectangle){ x - 40, y - 60, 80, 120 }, 0.2, 10, 2, GRAY);
    
    char letter[2] = { 'A' + pos, '\0' };
    DrawText(letter, x - 15, y - 30, 60, WHITE);
    DrawText(name, x - 15, y + 70, 20, LIGHTGRAY);
}

static void draw_lamp(int x, int y, char letter, bool on) {
    if (on) {
        DrawCircleGradient(x, y, 25, COLOR_LAMP_ON, (Color){ 200, 150, 0, 0 });
        DrawCircle(x, y, 20, COLOR_LAMP_ON);
        DrawText((char[]){letter, '\0'}, x - 8, y - 12, 24, BLACK);
    } else {
        DrawCircle(x, y, 20, COLOR_LAMP_OFF);
        DrawCircleLines(x, y, 20, GRAY);
        DrawText((char[]){letter, '\0'}, x - 8, y - 12, 24, GRAY);
    }
}

static void draw_key(int x, int y, char letter, bool pressed) {
    Rectangle rect = { x - 22, y - 22, 44, 44 };
    DrawRectangleRounded(rect, 0.2, 10, pressed ? GRAY : COLOR_KEY);
    DrawRectangleRoundedLines(rect, 0.2, 10, 2, pressed ? LIGHTGRAY : DARKGRAY);
    DrawText((char[]){letter, '\0'}, x - 8, y - 12, 24, WHITE);
}

void gui_run(EnigmaMachine* machine) {
    char last_out = 0;
    char last_in = 0;

    while (!WindowShouldClose()) {
        // Input Handling
        last_in = 0;
        for (int i = 'A'; i <= 'Z'; i++) {
            if (IsKeyDown(i)) {
                last_in = i;
                break;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE)) break;
        if (IsKeyPressed(KEY_BACKSPACE)) {
            resetMachine(machine);
            last_out = 0;
        }

        // Logic
        if (last_in != 0) {
            // We use IsKeyPressed for the actual encryption to prevent rapid-fire advance
            // But we use IsKeyDown for visual feedback of the key press
            if (IsKeyPressed(last_in)) {
                last_out = encryptChar(machine, last_in);
            }
        } else {
            last_out = 0;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(COLOR_BG);

        // Header
        DrawText("ENIGMA M3 SIMULATOR", 30, 30, 30, WHITE);
        DrawText("PRESS A-Z TO ENCRYPT | BACKSPACE TO RESET", 30, 70, 20, GRAY);

        // Rotors
        for (int i = 0; i < NUM_ROTORS; i++) {
            char name[8];
            snprintf(name, sizeof(name), "Rotor %c", machine->rotorOrder[i]);
            draw_rotor(200 + (NUM_ROTORS - 1 - i) * 150, 200, machine->rotors[i].currentPosition, name);
        }

        // Lampboard
        int lb_x = 100;
        int lb_y = 380;
        for (int r = 0; r < 3; r++) {
            int len = strlen(KB_ROWS[r]);
            for (int i = 0; i < len; i++) {
                char c = KB_ROWS[r][i];
                draw_lamp(lb_x + KB_OFFSETS[r] + i * 60, lb_y + r * 60, c, c == last_out);
            }
        }

        // Keyboard
        int kb_x = 100;
        int kb_y = 580;
        for (int r = 0; r < 3; r++) {
            int len = strlen(KB_ROWS[r]);
            for (int i = 0; i < len; i++) {
                char c = KB_ROWS[r][i];
                draw_key(kb_x + KB_OFFSETS[r] + i * 60, kb_y + r * 55, c, c == last_in);
            }
        }

        // Plugboard Panel
        DrawRectangleRounded((Rectangle){ 750, 130, 220, 400 }, 0.1, 10, COLOR_PANEL);
        DrawText("PLUGBOARD", 770, 150, 20, WHITE);
        int pb_idx = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (machine->plugboard.map[i] > i) {
                char pair[16];
                snprintf(pair, sizeof(pair), "%c <-> %c", 'A' + i, 'A' + machine->plugboard.map[i]);
                DrawText(pair, 780, 190 + pb_idx * 25, 18, LIGHTGRAY);
                pb_idx++;
            }
        }

        EndDrawing();
    }
}
