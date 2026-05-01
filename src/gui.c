#include "gui.h"
#include "cracker.h"
#include <raylib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

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
#define COLOR_ACCENT (Color){ 200, 50, 50, 255 }

static const char* KB_ROWS[] = {"QWERTZUIO", "ASDFGHJK", "PYXCVBNM"};
static int KB_OFFSETS[] = {0, 25, 50};

static void OpenLogFile(const char* filename) {
#if defined(_WIN32)
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "start %s", filename);
    system(cmd);
#elif defined(__APPLE__)
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "open %s", filename);
    system(cmd);
#else
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "xdg-open %s", filename);
    system(cmd);
#endif
}

void gui_init(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Enigma Machine Simulator - Raylib GUI");
    SetTargetFPS(60);
}

void gui_cleanup(void) {
    CloseWindow();
}

static void draw_rotor(int x, int y, int pos, const char* name) {
    DrawRectangleRounded((Rectangle){ x - 40, y - 60, 80, 120 }, 0.2, 10, COLOR_ROTOR);
    DrawRectangleRoundedLines((Rectangle){ x - 40, y - 60, 80, 120 }, 0.2, 10, GRAY);
    
    char letter[2] = { 'A' + pos, '\0' };
    DrawText(letter, x - 15, y - 30, 60, WHITE);
    DrawText(name, x - MeasureText(name, 20)/2, y + 70, 20, LIGHTGRAY);
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
    DrawRectangleRoundedLines(rect, 0.2, 10, pressed ? LIGHTGRAY : DARKGRAY);
    DrawText((char[]){letter, '\0'}, x - 8, y - 12, 24, WHITE);
}

void gui_run(EnigmaMachine* machine) {
    char last_out = 0;
    char last_in = 0;
    char plug_start = 0;
    
    // Cracking state
    bool cracking_mode = false;
    char crib_input[64] = "";
    int crib_len = 0;
    CrackerStatus crack_status = {0};

    // Log files opened once per session
    FILE *f_in = fopen("input.log", "a");
    FILE *f_out = fopen("output.log", "a");

    while (!WindowShouldClose()) {
        int sw = GetScreenWidth();
        int sh = GetScreenHeight();
        
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        char time_str[32];
        strftime(time_str, sizeof(time_str), "[%Y-%m-%d %H:%M:%S] ", t);
        
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
            if (IsKeyPressed(last_in)) {
                last_out = encryptChar(machine, last_in);
                if (f_in) { fprintf(f_in, "%s%c\n", time_str, last_in); fflush(f_in); }
                if (f_out) { fprintf(f_out, "%s%c\n", time_str, last_out); fflush(f_out); }
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
        // Left (0), Middle (1), Right (2)
        int rotor_base_x = sw / 2 - 150;
        for (int i = 0; i < NUM_ROTORS; i++) {
            char name[16];
            snprintf(name, sizeof(name), "Rotor %s", machine->rotorNames[i]);
            int rx = rotor_base_x + i * 150;
            int ry = 200;
            
            // Interaction: Clicking advances the rotor manually
            Rectangle rec = { rx - 40, ry - 60, 80, 120 };
            if (CheckCollisionPointRec(GetMousePosition(), rec) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                rotorStep(&machine->rotors[i]);
            }
            
            draw_rotor(rx, ry, machine->rotors[i].currentPosition, name);
        }

        // Lampboard
        int lb_x = sw / 2 - 270;
        int lb_y = 380;
        for (int r = 0; r < 3; r++) {
            int len = strlen(KB_ROWS[r]);
            for (int i = 0; i < len; i++) {
                char c = KB_ROWS[r][i];
                draw_lamp(lb_x + KB_OFFSETS[r] + i * 60, lb_y + r * 60, c, c == last_out);
            }
        }

        // Keyboard & Plugboard Logic
        int kb_x = sw / 2 - 270;
        int kb_y = 580;
        for (int r = 0; r < 3; r++) {
            int len = strlen(KB_ROWS[r]);
            for (int i = 0; i < len; i++) {
                char c = KB_ROWS[r][i];
                int kx = kb_x + KB_OFFSETS[r] + i * 60;
                int ky = kb_y + r * 55;
                
                // Interaction: Clicking a key with RIGHT MOUSE starts/ends a plug pair
                Rectangle krec = { kx - 22, ky - 22, 44, 44 };
                if (CheckCollisionPointRec(GetMousePosition(), krec) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                    if (plug_start == 0) {
                        plug_start = c;
                    } else {
                        if (plug_start != c) {
                            addPlugPairToMachine(machine, plug_start, c);
                        }
                        plug_start = 0;
                    }
                }
                
                draw_key(kx, ky, c, (c == last_in || c == plug_start));
            }
        }

        // Plugboard Panel
        int pb_w = 220;
        DrawRectangleRounded((Rectangle){ sw - pb_w - 30, 130, pb_w, 400 }, 0.1, 10, COLOR_PANEL);
        DrawText("PLUGBOARD", sw - pb_w - 10, 150, 20, WHITE);
        DrawText("Right-click keys to pair", sw - pb_w - 10, 175, 12, GRAY);
        
        int pb_idx = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (machine->plugboard.map[i] > i) {
                char pair[16];
                snprintf(pair, sizeof(pair), "%c <-> %c", 'A' + i, 'A' + machine->plugboard.map[i]);
                
                Rectangle p_rec = { sw - pb_w, 200 + pb_idx * 25, 200, 20 };
                bool p_hover = CheckCollisionPointRec(GetMousePosition(), p_rec);
                
                DrawText(pair, p_rec.x, p_rec.y, 18, p_hover ? WHITE : LIGHTGRAY);
                
                if (p_hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    removePlugPairFromMachine(machine, 'A' + i);
                }
                
                pb_idx++;
            }
        }

        // Action Buttons (Open Logs & Clear)
        int btn_y = 480;
        Rectangle btn_in = { sw - pb_w - 30, btn_y, pb_w, 35 };
        Rectangle btn_out = { sw - pb_w - 30, btn_y + 45, pb_w, 35 };
        Rectangle btn_clear = { sw - pb_w - 30, btn_y + 90, pb_w, 35 };

        // Open Input Log
        bool h_in = CheckCollisionPointRec(GetMousePosition(), btn_in);
        DrawRectangleRounded(btn_in, 0.2, 10, h_in ? GRAY : COLOR_KEY);
        DrawText("OPEN INPUT LOG", btn_in.x + 40, btn_in.y + 10, 14, WHITE);
        if (h_in && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) OpenLogFile("input.log");

        // Open Output Log
        bool h_out = CheckCollisionPointRec(GetMousePosition(), btn_out);
        DrawRectangleRounded(btn_out, 0.2, 10, h_out ? GRAY : COLOR_KEY);
        DrawText("OPEN OUTPUT LOG", btn_out.x + 40, btn_out.y + 10, 14, WHITE);
        if (h_out && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) OpenLogFile("output.log");

        // Clear Logs
        bool h_clear = CheckCollisionPointRec(GetMousePosition(), btn_clear);
        DrawRectangleRounded(btn_clear, 0.2, 10, h_clear ? COLOR_ACCENT : COLOR_KEY);
        DrawText("CLEAR ALL LOGS", btn_clear.x + 40, btn_clear.y + 10, 14, WHITE);
        
        if (h_clear && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (f_in) freopen("input.log", "w", f_in);
            if (f_out) freopen("output.log", "w", f_out);
        }

        // Cracking Interface
        Rectangle btn_crack_toggle = { 30, sh - 100, 200, 40 };
        bool h_crack = CheckCollisionPointRec(GetMousePosition(), btn_crack_toggle);
        DrawRectangleRounded(btn_crack_toggle, 0.2, 10, cracking_mode ? COLOR_ACCENT : COLOR_KEY);
        DrawText(cracking_mode ? "EXIT CRACKER" : "OPEN CRACKER", btn_crack_toggle.x + 30, btn_crack_toggle.y + 10, 16, WHITE);
        
        if (h_crack && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            cracking_mode = !cracking_mode;
        }

        if (cracking_mode) {
            DrawRectangle(0, 0, sw, sh, (Color){ 0, 0, 0, 200 }); // Overlay
            DrawRectangleRounded((Rectangle){ sw/2 - 250, sh/2 - 150, 500, 300 }, 0.1, 10, COLOR_PANEL);
            DrawText("AUTO-DECRYPT (BRUTE FORCE)", sw/2 - 180, sh/2 - 130, 24, WHITE);
            
            DrawText("ENTER CRIB (Guessed Plaintext):", sw/2 - 230, sh/2 - 80, 18, GRAY);
            DrawRectangle(sw/2 - 230, sh/2 - 50, 460, 40, BLACK);
            DrawRectangleLines(sw/2 - 230, sh/2 - 50, 460, 40, GRAY);
            DrawText(crib_input, sw/2 - 220, sh/2 - 40, 20, YELLOW);

            // Handle Crib Typing
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (crib_len < 63)) {
                    crib_input[crib_len] = (char)toupper(key);
                    crib_input[++crib_len] = '\0';
                }
                key = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && crib_len > 0) {
                crib_input[--crib_len] = '\0';
            }

            // Start Crack Button
            Rectangle btn_run = { sw/2 - 100, sh/2 + 20, 200, 40 };
            bool h_run = CheckCollisionPointRec(GetMousePosition(), btn_run);
            DrawRectangleRounded(btn_run, 0.2, 10, h_run ? GREEN : DARKGREEN);
            DrawText("RUN CRACKER", btn_run.x + 40, btn_run.y + 10, 16, WHITE);

            if (h_run && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && crib_len > 2) {
                // To keep it simple for this version, we read the output.log as our "captured ciphertext"
                // In a real scenario, this would be the message you're trying to break.
                FILE* f = fopen("output.log", "r");
                if (f) {
                    char captured[1024] = "";
                    char line[256];
                    while (fgets(line, sizeof(line), f)) {
                        // Extract char from log "[YYYY...] C"
                        char* last_space = strrchr(line, ' ');
                        if (last_space && isalpha(last_space[1])) {
                            strncat(captured, &last_space[1], 1);
                        }
                    }
                    fclose(f);

                    if (strlen(captured) > 0) {
                        crack_status.ciphertext = captured;
                        crack_status.crib = crib_input;
                        crack_code(&crack_status); // This is blocking for now, but fast enough for M3

                        if (crack_status.success) {
                            // Apply found settings
                            const char* found_names[3] = { crack_status.found_rotor_names[0], crack_status.found_rotor_names[1], crack_status.found_rotor_names[2] };
                            setRotorOrder(machine, found_names);
                            setStartPositions(machine, crack_status.found_start_pos);
                            resetMachine(machine);
                            cracking_mode = false;
                        }
                    }
                }
            }
            
            if (crack_status.progress > 0 && !crack_status.success) {
                DrawText("No match found. Try a different crib.", sw/2 - 150, sh/2 + 80, 16, RED);
            }
        }

        EndDrawing();
    }

    if (f_in) fclose(f_in);
    if (f_out) fclose(f_out);
}
