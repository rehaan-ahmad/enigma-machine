# 📟 Enigma Machine Simulator (M3)

[![C99](https://img.shields.io/badge/Language-C99-blue.svg)](https://en.wikipedia.org/wiki/C99)
[![ncurses](https://img.shields.io/badge/UI-ncurses-green.svg)](https://invisible-island.net/ncurses/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

> **"Can you crack the code?"**  
> Step into the shoes of a WWII cryptographer. This project is a high-fidelity C implementation of the legendary **Enigma M3** machine, used by the Wehrmacht and Heer. From the signature double-stepping anomaly to the complex internal wiring, every detail has been meticulously simulated to bring history to your terminal.

---

## ✨ Features

- **🎯 Historical Accuracy**: Fully simulated Rotor (I-V) and Reflector (UKW-B) wirings.
- **⚙️ Double-Stepping**: Accurate implementation of the historical mechanical stepping anomaly.
- **🖥️ Interactive TUI**: A rich, "retro-hacker" terminal interface built with `ncurses`.
- **🔌 Virtual Plugboard**: Full support for bidirectional character swapping.
- **📁 Preset System**: Save and load your complex machine configurations using JSON.
- **⌨️ CLI Mode**: Power-user support for headless encryption, decryption, and piping.
- **✅ Self-Inverse Logic**: Perfectly replicates the Enigma's property where encryption and decryption are the same process.

---

## 🚀 Getting Started

### Prerequisites

To build and run the simulator, you'll need:
- **GCC** (or any C99-compliant compiler)
- **Make**
- **ncurses** development libraries:
  - Ubuntu/Debian: `sudo apt-get install libncurses5-dev libncursesw5-dev`
  - Fedora: `sudo dnf install ncurses-devel`
  - macOS: `brew install ncurses`

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

3. (Optional) Run the automated logic tests:
   ```bash
   ./enigma --test
   ```

---

## 🛠️ Usage

### 🎮 Interactive TUI (Default)
Simply run the executable to launch the full-screen interactive mode:
```bash
./enigma
```
- **Type A-Z**: Encrypt characters in real-time.
- **'|'**: Reset the machine to its start positions.
- **'ESC'**: Exit the application.

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

The simulator is built with a modular C architecture for clarity and performance:

- `src/enigma.c`: The "brain" of the machine, managing the signal flow.
- `src/rotor.c`: Handles wiring rotations and the stepping mechanism.
- `src/ui.c`: Manages the `ncurses` lifecycle and visual components.
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
- Built with ❤️ using the [cJSON](https://github.com/DaveGamble/cJSON) library.

---
*Disclaimer: This is a historical simulation intended for educational purposes only.*
