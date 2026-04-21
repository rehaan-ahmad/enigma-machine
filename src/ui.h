#ifndef UI_H
#define UI_H

#include "enigma.h"

// Initialize the ncurses TUI
void ui_init(void);

// Run the main TUI loop
void ui_run(EnigmaMachine* machine);

// Clean up ncurses
void ui_cleanup(void);

#endif // UI_H
