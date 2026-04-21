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
- [ ] Hardcode historical rotors VI–VIII (Kriegsmarine)
- [x] Implement `rotor_forward(rotor, signal)` — signal through wiring left-to-right
- [x] Implement `rotor_backward(rotor, signal)` — signal right-to-left (return path)
- [x] Implement `rotor_step(rotor[3])` — double-stepping anomaly included
  - Middle rotor steps when it's at notch AND right rotor steps
  - Middle rotor steps again next tick (double-step)

### Reflector
- [x] Define reflector struct: `wiring[26]`
- [x] Hardcode UKW-B
- [ ] Hardcode UKW-A, UKW-C
- [x] Implement `reflect(reflector, signal)`

### Plugboard
- [x] Define plugboard struct: `pairs[10][2]`, `map[26]`
- [x] Implement `plugboard_swap(plugboard, c)` — bidirectional
- [x] Validate: no letter paired twice, max 10 pairs

### Cipher Core
- [x] Implement `enigma_encrypt_char(machine, c)`:
  1. Plugboard swap
  2. Through rotors R → M → L (forward)
  3. Reflect
  4. Through rotors L → M → R (backward)
  5. Plugboard swap again
  6. Step rotors BEFORE encryption (historical order)
- [x] Implement `enigma_encrypt_string(machine, input, output)`
- [x] Verify: encrypting ciphertext with same settings = original plaintext (self-inverse)

---

## Phase 2 — Preset System

### Preset File Format (JSON)
```json
{
  "name": "MyPreset",
  "rotors": ["I", "II", "III"],
  "ring_settings": [1, 1, 1],
  "start_positions": ["A", "A", "Z"],
  "reflector": "UKW-B",
  "plugboard": [["A","Z"], ["B","Y"]]
}
```

- [ ] Implement `preset_save(preset, filename)` — write JSON to `presets/`
- [ ] Implement `preset_load(filename, preset)` — parse JSON, validate fields
- [ ] Implement `preset_list()` — scan `presets/` directory, return names
- [ ] Implement `preset_delete(name)`
- [ ] Use `cJSON` (single-header, include in `src/`) for JSON parsing

### Encrypt/Decrypt Flow
- [ ] Load preset → set machine state exactly as saved
- [ ] Encrypt string → output ciphertext
- [ ] To decrypt: reload SAME preset (resets positions) → feed ciphertext → get plaintext
- [ ] **Key insight**: Enigma is self-inverse; no separate decrypt function needed

---

## Phase 3 — TUI (ncurses)

### Screens

#### Main Menu
```
┌─────────────────────────────┐
│      ENIGMA MACHINE         │
├─────────────────────────────┤
│  [1] Encrypt / Decrypt      │
│  [2] Configure Machine      │
│  [3] Manage Presets         │
│  [4] Quit                   │
└─────────────────────────────┘
```

#### Encrypt / Decrypt Screen
- [ ] Preset selector (arrow keys, press Enter)
- [ ] Text input field (filtered to A–Z, spaces stripped or kept)
- [ ] Live output panel — show ciphertext as user types
- [ ] Show current rotor positions updating in real-time
- [ ] Copy-to-clipboard hint (pipe to `xclip` or display copyable block)

#### Configure Machine Screen
- [ ] Rotor slot selector (L, M, R) — cycle through I–VIII with arrow keys
- [ ] Ring setting input per rotor (1–26)
- [ ] Start position input per rotor (A–Z)
- [ ] Reflector selector (UKW-A/B/C)
- [ ] Plugboard editor — add/remove pairs interactively
- [ ] Save As Preset button → prompt for name

#### Preset Manager Screen
- [ ] Scrollable list of saved presets
- [ ] [L] Load, [D] Delete, [N] New preset from current config
- [ ] Show preset summary on selection

### TUI Implementation
- [ ] Link against `ncurses` (`-lncurses` in Makefile)
- [ ] `ui_init()` — `initscr()`, color pairs, `keypad(stdscr, TRUE)`, `noecho()`
- [ ] `ui_cleanup()` — `endwin()`
- [ ] Draw borders with `box()` or manual `ACS_*` chars
- [ ] Color scheme: green-on-black (Enigma terminal aesthetic)
  ```c
  init_pair(1, COLOR_GREEN, COLOR_BLACK);   // Normal text
  init_pair(2, COLOR_BLACK, COLOR_GREEN);   // Selected/highlight
  init_pair(3, COLOR_RED,   COLOR_BLACK);   // Errors
  init_pair(4, COLOR_CYAN,  COLOR_BLACK);   // Rotor display
  ```
- [ ] Handle terminal resize with `KEY_RESIZE`

---

## Phase 4 — CLI Mode (Non-interactive)

For scripting / pipe usage:

```bash
./enigma --preset MyPreset --encrypt "HELLO WORLD"
./enigma --preset MyPreset --decrypt "MFNCZBBFZM"
./enigma --preset MyPreset --encrypt < input.txt > output.txt
./enigma --list-presets
```

- [ ] Parse `argc/argv` before launching TUI
- [ ] If `--encrypt` or `--decrypt` flag present, run headless and exit
- [ ] If no flags, launch TUI

---

## Phase 5 — Build & Polish

### Makefile
- [x] Basic build system working
- [ ] Link against `ncurses` (when UI is added)

### Testing
- [x] Unit test `enigma_encrypt_char` self-inverse property
- [x] Test double-stepping: rotor sequence ADV → step → AEW → BFX
- [ ] Test known ciphertext: Wehrmacht message decode (historical test vectors)
- [ ] Test preset round-trip: save → load → encrypt → decrypt = original

### Dependencies
- `ncurses` (system: `libncurses-dev` on Ubuntu / `ncurses-devel` on Fedora)
- `cJSON` — copy `cJSON.c` and `cJSON.h` into `src/` (MIT license, no install needed)

---

## Historical Rotor Wirings Reference

| Rotor | Wiring (A→Z)               | Notch |
|-------|----------------------------|-------|
| I     | EKMFLGDQVZNTOWYHXUSPAIBRCJ | Q     |
| II    | AJDKSIRUXBLHWTMCQGZNPYFVOE | E     |
| III   | BDFHJLCPRTXVZNYEIWGAKMUSQO | V     |
| IV    | ESOVPZJAYQUIRHXLNFTGKDCMWB | J     |
| V     | VZBRGITYUPSDNHLXAWMJQOFECK | Z     |

| Reflector | Wiring (A→Z)               |
|-----------|----------------------------|
| UKW-B     | YRUHQSLDPXNGOKMIEBFZCWVJAT |
| UKW-C     | FVPJIAOYEDRZXWGCTKUQSBNMHL |

---

## Completion Checklist

- [x] Phase 1: Core engine working, self-inverse verified
- [ ] Phase 2: Presets save/load, round-trip encrypt/decrypt working
- [ ] Phase 3: TUI functional on Linux terminal (Fedora compatible)
- [ ] Phase 4: CLI mode working
- [ ] Phase 5: Makefile builds clean, historical test vectors pass
