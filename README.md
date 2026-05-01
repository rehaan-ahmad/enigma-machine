# 📟 Enigma Machine Simulator (M3)

[![C99](https://img.shields.io/badge/Language-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![Raylib](https://img.shields.io/badge/GUI-Raylib-red.svg)](https://www.raylib.com/)
[![ncurses](https://img.shields.io/badge/TUI-ncurses-green.svg)](https://invisible-island.net/ncurses/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

> **"Can you crack the code?"**  
> Step into the shoes of a WWII cryptographer. This project is a high-fidelity C implementation of the legendary **Enigma M3** machine, used by the Wehrmacht and Heer. From the signature double-stepping anomaly to the complex internal wiring, every detail has been meticulously simulated.

---

## ✨ Features

- **🎨 Modern Graphical Interface (NEW!)**: A hardware-accelerated GUI built with **Raylib**, featuring glowing lamps, 3D-style rotors, and interactive keys.
- **🎯 Historical Accuracy**: Fully simulated Rotor (I-V) and Reflector (UKW-B) wirings.
- **⚙️ Double-Stepping**: Accurate implementation of the historical mechanical stepping anomaly.
- **🖥️ Retro TUI**: A character-based terminal interface built with `ncurses` for that classic hacker feel.
- **📁 Preset System**: Save and load your complex machine configurations using JSON via `cJSON`.
- **⌨️ CLI Mode**: Power-user support for headless encryption, decryption, and piping.
- **✅ Self-Inverse Logic**: Perfectly replicates the Enigma's property where encryption and decryption are the same process.

---

## 🚀 Getting Started

### Prerequisites

To build and run the simulator, you'll need:
- **GCC** (or any C99-compliant compiler)
- **Make**
- **Raylib** (for the GUI experience):
  - Ubuntu/Debian: `sudo apt install libraylib-dev`
  - Fedora: `sudo dnf install raylib-devel`
  - macOS: `brew install raylib`
- **ncurses** (for the TUI experience):
  - Ubuntu/Debian: `sudo apt install libncurses5-dev`

### Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/rehaan-ahmad/enigma-machine.git
   cd enigma-machine
   ```

2. Compile the project:
   ```bash
   make
   ```
   > **Note**: Our **Smart Build System** automatically detects if Raylib is installed. If missing, it will gracefully fallback to the TUI-only version.

---

## 🛠️ Usage

### 🎮 Interactive Mode (Default)
Simply run the executable to launch the interface. It will default to **GUI** if Raylib is available, otherwise it will launch the **TUI**:
```bash
./enigma
```
- **Explicit GUI**: `./enigma --gui`
- **Explicit TUI**: `./enigma --tui`

### 💻 Command Line Mode
Perform quick encryptions or batch processing:
```bash
# Direct encryption
./enigma --encrypt "HELLO WORLD"

# Using a saved preset
./enigma --preset "MySecretConfig" --encrypt "QSV"

# Piping support
echo "TOP SECRET" | ./enigma
```

---

## 🏗️ Architecture

The simulator is built with a modular C architecture:

- `src/enigma.c`: The "brain" of the machine, managing signal flow.
- `src/gui.c`: Modern Raylib-based graphical rendering.
- `src/ui.c`: Character-based `ncurses` terminal interface.
- `src/rotor.c`: Handles wiring rotations and the stepping mechanism.
- `src/preset.c`: Serialization of machine state via `cJSON`.

---

## 🧪 Historical Verification

This simulator has been verified against known historical test vectors:
- **Input**: `AAAAA`
- **Settings**: Rotors I, II, III | Reflector B | Start AAA | Ring 01.01.01
- **Output**: `BDZGO` ✅

---

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

## 🙌 Acknowledgments

- Dedicated to the cryptanalysts at Bletchley Park who changed the course of history.
- Built with ❤️ using [Raylib](https://github.com/raysan5/raylib) and [cJSON](https://github.com/DaveGamble/cJSON).

---
*Disclaimer: This is a historical simulation intended for educational purposes only.*
