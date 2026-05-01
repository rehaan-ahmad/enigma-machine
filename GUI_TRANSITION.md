# Transitioning from TUI to GUI: Implementation Plan

Moving the Enigma Machine Simulator from a Terminal User Interface (TUI) to a Graphical User Interface (GUI) involves several key architectural and technical shifts. To ensure compatibility across Windows, macOS, and Linux, we must choose cross-platform tools and decouple the core logic from the presentation layer.

## 1. Architectural Changes

### From Loop-Based to Event-Driven
*   **Current (TUI):** A `while(1)` loop in `ui_run` that waits for a keypress, updates state, and redraws the whole screen.
*   **GUI:** An asynchronous **Event Loop**. The application waits for signals (clicks, keypresses, window events). Redrawing is handled by the OS/Library when "dirty" regions are identified.

### State Management
*   The `EnigmaMachine` struct is currently passed around. In a GUI, we might need a **Controller** or **ViewModel** that wraps the C struct and notifies the UI when the machine's state (e.g., rotor positions) changes.

---

## 2. Recommended Technology Stack

For a C-based project requiring cross-platform compatibility, here are the top contenders:

| Option | Pros | Cons |
| :--- | :--- | :--- |
| **Qt (C++)** | Industry standard, powerful layout engine, native look and feel on all OSs. | Large binary size, requires C++ wrapper for our C core. |
| **GTK (C)** | Native C API, standard on Linux, works well on Windows/Mac. | Windows/Mac setup can be finicky; doesn't always feel "native" outside Linux. |
| **Raylib (C)** | Extremely simple, "game-like" GUI, single-header feel, hardware accelerated. | Custom UI widgets (buttons, sliders) must be built or added via libraries (e.g., raygui). |
| **Tauri / Electron** | Modern web tech (HTML/CSS/JS) for the frontend. | High memory usage; requires a bridge between C and JS. |

**Recommendation:** **Qt** or **Raylib**. Qt is best for a professional "app" feel; Raylib is best if we want a highly visual, animated Enigma machine with a retro aesthetic.

---

## 3. Component Breakdown: What Changes?

### `src/ui.c` & `src/ui.h` [REPLACE]
These files are currently tightly coupled with `ncurses`. 
*   **TUI version:** Uses `mvprintw` and `attron`.
*   **GUI version:** Will use `drawTexture`, `Label`, `Button`, and `layout` components. 
*   **Assets:** We will need images (PNGs) for the Enigma case, rotors, and lamps, or use vector graphics (SVG).

### `src/main.c` [MODIFY]
*   Instead of calling `ui_init()`, it will initialize the GUI framework.
*   It must handle command-line arguments to support *both* the CLI mode and the GUI mode (e.g., `--gui`).

### `Makefile` [MODIFY]
*   **Dependencies:** Link against GUI libraries (e.g., `-lraylib` or Qt frameworks).
*   **Platform Detection:** Add logic to handle different compiler flags for `MINGW` (Windows), `Darwin` (Mac), and `Linux`.
*   **Resource Bundling:** On Mac, we'll need to create a `.app` bundle; on Windows, an `.exe` with icon embedding.

---

## 4. New GUI Features
The TUI is limited by character cells. A GUI allows for:
1.  **Visual Rotors:** Showing the actual wiring or 3D-like wheels that "click" into place.
2.  **Drag-and-Drop:** Physically moving rotors into slots.
3.  **Interactive Plugboard:** Clicking on sockets to drag wires between them (instead of typing pairs).
4.  **Lampboard Glow:** Smooth transitions and lighting effects when a key is pressed.
5.  **Output Export:** A text area to copy/paste long encrypted messages.

---

## 5. Migration Roadmap

1.  **Refactor Core Logic:** Ensure `enigma.c` and its dependencies have zero IO (no `printf`). They should only manipulate data.
2.  **Choose Library:** Select a GUI framework (e.g., Raylib for simplicity).
3.  **Create "Headless" GUI:** Get a window to open with a simple "Hello Enigma" text using the chosen library.
4.  **Implement Keyboard Input:** Map physical keypresses to `encryptChar`.
5.  **Visual Layout:** Build the Lampboard, then the Keyboard, then the Rotor settings.
6.  **Cross-Compilation:** Set up CI (GitHub Actions) to build binaries for all three platforms.
