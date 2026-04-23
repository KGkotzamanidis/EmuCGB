### **A High-Performance Game Boy & Game Boy Color Emulator**
> **Built with C++ for precision and speed.**

[![C++](https://img.shields.io/badge/C++-17-blue.svg?style=for-the-badge&logo=c%2B%2B)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux-lightgrey.svg?style=for-the-badge)](https://github.com/KGkotzamanidis/EmuCGB)
[![License](https://img.shields.io/badge/License-GPLv3-green.svg?style=for-the-badge)](LICENSE)

---

## 🌟 Overview

**EmuCGB** is a from-scratch emulation project designed to recreate the hardware logic of the **Nintendo Game Boy (DMG)** and **Game Boy Color (CGB)**. It focuses on modular code architecture, making it a great resource for learning about systems programming.

---

## 🚀 Key Features

* **🕹️ Core Emulation**: Full Sharp LR35902 CPU instruction set implementation.
* **🎨 CGB Support**: *(Work in Progress)* Vibrant 15-bit color palettes and double-speed mode.
* **💾 Memory Banking**: Support for `MBC1`, `MBC3+RTC` controllers.
* **📺 Pixel Processing**: Cycle-accurate PPU rendering for backgrounds and sprites.
* **⌨️ Responsive Input**: *(Work in Progress)* Low-latency mapping for keyboard and controllers.
* **🛠️ Debug Ready**: Clean internal bus structure for easy state inspection.

---

## 🏗️ System Architecture

| Component | Responsibility |
| :--- | :--- |
| **🧠 CPU** | Instruction decoding, registers, and interrupt handling. |
| **📟 PPU** | Scanline rendering, OAM, and VRAM management. |
| **⚡ Bus/MMU** | 16-bit address routing and I/O register control. |
| **⏰ Timer** | Accurate DIV and TIMA frequency management. |
| **🎧 APU** | *(Work in Progress)* Pulse, Wave, and Noise channels. |

---

## 🛠️ Installation & Build

### 📋 Prerequisites
* **Compiler**: C++17 or higher
* **Build Tool**: [Make](https://www.gnu.org/software/make/) (v4.4.1)
* **Library**: [SDL3](https://www.libsdl.org/)

### 🔧 Build Instructions
```bash
# Clone the repository
git clone https://github.com/KGkotzamanidis/EmuCGB.git
cd EmuCGB

# Download the Libraries
make install

# Build the Project
make build

# Run the Project
make run