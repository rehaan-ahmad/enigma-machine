#include "ui.h"
#include <ncurses.h>
#include <string.h>
#include <ctype.h>

#define COLOR_ROTOR 1
#define COLOR_LAMP_ON 2
#define COLOR_LAMP_OFF 3
#define COLOR_KEY 4

static void setup_colors() {
    start_color();
    init_pair(COLOR_ROTOR, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_LAMP_ON, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_LAMP_OFF, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_KEY, COLOR_GREEN, COLOR_BLACK);
}

static void draw_rotor(int y, int x, int pos, const char* name) {
    attron(COLOR_PAIR(COLOR_ROTOR) | A_BOLD);
    mvprintw(y, x, "[ %s ]", name);
    mvprintw(y + 1, x, "  %c  ", 'A' + pos);
    attroff(COLOR_PAIR(COLOR_ROTOR) | A_BOLD);
}

static void draw_lamp(int y, int x, char letter, bool on) {
    if (on) {
        attron(COLOR_PAIR(COLOR_LAMP_ON) | A_BOLD | A_UNDERLINE);
    } else {
        attron(COLOR_PAIR(COLOR_LAMP_OFF));
    }
    mvprintw(y, x, "( %c )", letter);
    attroff(COLOR_PAIR(COLOR_LAMP_ON) | A_BOLD | A_UNDERLINE | COLOR_PAIR(COLOR_LAMP_OFF));
}

static void draw_keyboard(int y, int x) {
    const char* rows[] = {"QWERTZUIO", "ASDFGHJK", "PYXCVBNM"};
    int offsets[] = {0, 2, 4};
    
    for (int r = 0; r < 3; r++) {
        for (int i = 0; i < (int)strlen(rows[r]); i++) {
            mvprintw(y + r, x + offsets[r] + i * 4, "[ %c ]", rows[r][i]);
        }
    }
}

static void draw_lampboard(int y, int x, char highlight) {
    const char* rows[] = {"QWERTZUIO", "ASDFGHJK", "PYXCVBNM"};
    int offsets[] = {0, 2, 4};
    
    for (int r = 0; r < 3; r++) {
        for (int i = 0; i < (int)strlen(rows[r]); i++) {
            draw_lamp(y + r, x + offsets[r] + i * 5, rows[r][i], rows[r][i] == highlight);
        }
    }
}

void ui_init(void) {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    setup_colors();
}

void ui_cleanup(void) {
    endwin();
}

void ui_run(EnigmaMachine* machine) {
    int ch;
    char last_out = 0;
    
    while (1) {
        clear();
        
        // Header
        attron(A_BOLD | A_UNDERLINE);
        mvprintw(1, 10, "ENIGMA MACHINE SIMULATOR - M3 (Wermacht/Heer)");
        attroff(A_BOLD | A_UNDERLINE);
        
        // Rotors
        mvprintw(4, 5, "ROTORS:");
        for (int i = 0; i < NUM_ROTORS; i++) {
            char nameStr[8];
            snprintf(nameStr, sizeof(nameStr), "%c", machine->rotorOrder[i]);
            draw_rotor(5, 5 + (NUM_ROTORS - 1 - i) * 10, machine->rotors[i].currentPosition, nameStr);
        }
        
        // Lampboard
        mvprintw(10, 5, "LAMPBOARD:");
        draw_lampboard(11, 5, last_out);
        
        // Keyboard
        mvprintw(16, 5, "KEYBOARD:");
        draw_keyboard(17, 5);
        
        // Plugboard
        mvprintw(5, 45, "PLUGBOARD:");
        int pb_idx = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (machine->plugboard.map[i] > i) {
                mvprintw(6 + pb_idx, 45, "%c <-> %c", 'A' + i, 'A' + machine->plugboard.map[i]);
                pb_idx++;
            }
        }
        
        // Instructions
        mvprintw(22, 5, "TYPE A-Z TO ENCRYPT | ESC TO QUIT | '|' TO RESET");
        
        refresh();
        
        ch = getch();
        if (ch == 27) break; // ESC
        if (ch == '|') {
            resetMachine(machine);
            last_out = 0;
            continue;
        }
        
        if (isalpha(ch)) {
            last_out = encryptChar(machine, toupper(ch));
        } else {
            last_out = 0;
        }
    }
}
