# Enigma Machine — C Implementation TODO

## Architecture Overview

```
enigma/
├── src/
│   ├── main.c          # Entry point, TUI loop
│   ├── enigma.c/.h     # Core machine logic
│   ├── rotor.c/.h      # Rotor wiring + stepping
│   ├── plugboard.c/.h  # Plugboard swap logic
│   ├── reflector.c/.h  # Reflector wiring
│   ├── preset.c/.h     # Save/load presets
│   └── ui.c/.h         # ncurses TUI
├── presets/            # .enigma preset files (JSON)
├── Makefile
└── README.md
```

---

## Phase 1 — Core Engine

### Rotors
- [x] Define rotor struct: `wiring[26]`, `notch`, `ring_setting`, `position`
- [x] Hardcode historical rotors I–V (Wehrmacht)
- [x] Implement `rotor_forward(rotor, signal)` — signal through wiring left-to-right
- [x] Implement `rotor_backward(rotor, signal)` — signal right-to-left (return path)
- [x] Implement `rotor_step(rotor[3])` — double-stepping anomaly included
  - Middle rotor steps when it's at notch AND right rotor steps
  - Middle rotor steps again next tick (double-step)

### Reflector
- [x] Define reflector struct: `wiring[26]`
- [x] Hardcode UKW-B
- [x] Implement `reflect(reflector, signal)`

### Plugboard
- [x] Define plugboard struct: `pairs[10][2]`, `map[26]`
- [x] Implement `plugboard_swap(plugboard, c)` — bidirectional
- [x] Validate: no letter paired twice, max 10 pairs

### Cipher Core
- [x] Implement `enigma_encrypt_char(machine, c)`:
  1. Step rotors BEFORE encryption (historical order)
  2. Plugboard swap
  3. Through rotors R → M → L (forward)
  4. Reflect
  5. Through rotors L → M → R (backward)
  6. Plugboard swap again
- [x] Implement `enigma_encrypt_string(machine, input, output)`
- [x] Verify: encrypting ciphertext with same settings = original plaintext (self-inverse)

---

## Phase 2 — Preset System

### Preset File Format (JSON)
- [x] Implement `preset_save(preset, filename)` — write JSON to `presets/`
- [x] Implement `preset_load(filename, preset)` — parse JSON, validate fields
- [x] Implement `preset_list()` — scan `presets/` directory, return names
- [x] Implement `preset_delete(name)`
- [x] Use `cJSON` for JSON parsing

### Encrypt/Decrypt Flow
- [x] Load preset → set machine state exactly as saved
- [x] Encrypt string → output ciphertext
- [x] To decrypt: reload SAME preset (resets positions) → feed ciphertext → get plaintext

---

## Phase 3 — TUI (ncurses)

### Screens
- [x] Main TUI layout with Rotor, Lampboard, and Keyboard visualization
- [x] Live output panel — show ciphertext highlight on lampboard as user types
- [x] Show current rotor positions updating in real-time
- [x] Interactive configuration (Reset with 'R')

### TUI Implementation
- [x] Link against `ncurses` (`-lncurses` in Makefile)
- [x] `ui_init()` — `initscr()`, color pairs, `keypad(stdscr, TRUE)`, `noecho()`
- [x] `ui_cleanup()` — `endwin()`
- [x] Draw borders and components with ncurses primitives
- [x] Color scheme: Cyan rotors, Yellow active lamps, Green instructions

---

## Phase 4 — CLI Mode (Non-interactive)

- [x] Parse `argc/argv` with `getopt_long`
- [x] Headless mode: `--encrypt "TEXT"`
- [x] Preset support in CLI: `--preset "Name"`
- [x] Stdin piping support: `echo "TEXT" | ./enigma`
- [x] Help menu: `--help`

---

## Phase 5 — Build & Polish

### Makefile
- [x] Basic build system working
- [x] Link against `ncurses` and `cJSON` correctly

### Testing
- [x] Unit test `enigma_encrypt_char` self-inverse property
- [x] Test double-stepping: rotor sequence ADV → step → AEW → BFX
- [x] Test known ciphertext: Wehrmacht message decode (historical test vectors)
- [x] Test preset round-trip: save → load → encrypt → decrypt = original

### Dependencies
- [x] `ncurses` (system: `libncurses-dev` on Ubuntu / `ncurses-devel` on Fedora)
- [x] `cJSON` — integrated into `src/`

---

## Completion Checklist

- [x] Phase 1: Core engine working, self-inverse verified
- [x] Phase 2: Presets save/load, round-trip encrypt/decrypt working
- [x] Phase 3: TUI functional on Linux terminal
- [x] Phase 4: CLI mode working
- [x] Phase 5: Makefile builds clean, historical test vectors pass
