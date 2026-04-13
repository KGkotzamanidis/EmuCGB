/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Mon Apr 07 2025>  <KGkotzamanidis>
 *
 *This program is free software: you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *
 *You should have received a copy of the GNU General Public License
 *along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "MMU.h"
MMU::MMU(BIOS &bios, ROM &rom, Interrupts &interrupts, Timers &timers, WRAM &wram) : bios(&bios), rom(&rom), interrupts(&interrupts), timers(&timers), wram(&wram) {
    std::printf("-=MMU class initialized=-\n");
}

uint8_t MMU::readByte(uint16_t address) {
    uint8_t data = 0xFF;

    if (address >= 0x0000 && address <= 0x7FFF) {
        if (address < 0x0100 && (bios->isBIOSLoaded && bios->BootBIOS)) { // 16 KiB ROM
            data = bios->receivingData(address);
        } else {
            data = rom->receivingData(address);
        }
    } else if (address >= 0x8000 && address <= 0x9FFF) { // 8 KiB Video RAM (VRAM)
    } else if (address >= 0xA000 && address <= 0xBFFF) { // 8 KiB External RAM
        data = rom->receivingData(address);
    } else if (address >= 0xC000 && address <= 0xFDFF) { // 4 KiB Work RAM (WRAM)
        data = wram->receivingData(address);
    } else if (address >= 0xFE00 && address <= 0xFE9F) {     // Object Attribute Memory (OAM)
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {     // NOT USED!
    } else if (address >= 0xFF00 && address <= 0xFF70) {     // I/O Registers
        if (address == 0xFF00) {                             // Joypad Input
        } else if (address >= 0xFF01 && address <= 0xFF02) { // Serial Transfer
        } else if (address >= 0xFF04 && address <= 0xFF07) { // Timer and Divider
            data = timers->receivingData(address);
        } else if (address == 0xFF0F) { // Interrupts
            data = interrupts->Registers.IF;
        } else if (address >= 0xFF10 && address <= 0xFF26) {                   // Audio
        } else if (address >= 0xFF30 && address <= 0xFF3F) {                   // Wave Pattern
        } else if (address >= 0xFF40 && address <= 0xFF4B) {                   // LCD Controller
        } else if (address == 0xFF46) {                                        // OAM DMA Transfer
        } else if ((address >= 0xFF4C && address <= 0xFF4D) && rom->CGBmode) { // KEY0 & KEY1
            if (address == 0xFF4C) {
                data = KEY_0;
            } else if (address == 0xFF4D) {
                data = KEY_1;
            }
        } else if (address == 0xFF4F && rom->CGBmode) {                        // VRAM Bank Select
        } else if (address == 0xFF50) {                                        // Boot Rom
        } else if ((address >= 0xFF51 && address <= 0xFF55) && rom->CGBmode) { // VRAM DMA
        } else if (address == 0xFF56 && rom->CGBmode) {                        // IR Port
        } else if ((address >= 0xFF68 && address <= 0xFF6B) && rom->CGBmode) { // BG-OBJ Palettes
        } else if (address == 0xFF6C && rom->CGBmode) {                        // Object Priority Mode
        } else if (address == 0xFF70 && rom->CGBmode) {                        // WRAM Bank Select
            data = wram->receivingData(address);
        }
    } else if (address >= 0xFF80 && address <= 0xFFFE) { // High RAM (HRAM)
        data = HRAM[address & 0x7F];
    } else if (address == 0xFFFF) { // Interrupt Enable Register
        data = interrupts->Registers.IE;
    }

    return data;
}
void MMU::writeByte(uint16_t address, uint8_t data) { // 16 KiB ROM
    if (address >= 0x0000 && address <= 0x7FFF) {
    } else if (address >= 0x8000 && address <= 0x9FFF) { // 8 KiB Video RAM (VRAM)
    } else if (address >= 0xA000 && address <= 0xBFFF) { // 8 KiB External RAM
    } else if (address >= 0xC000 && address <= 0xFDFF) { // 4 KiB Work RAM (WRAM)
        wram->sendingData(address, data);
    } else if (address >= 0xFE00 && address <= 0xFE9F) {     // Object Attribute Memory (OAM)
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {     // NOT USED!
    } else if (address >= 0xFF00 && address <= 0xFF70) {     // I/O Registers
        if (address == 0xFF00) {                             // Joypad Input
        } else if (address >= 0xFF01 && address <= 0xFF02) { // Serial Transfer
        } else if (address >= 0xFF04 && address <= 0xFF07) { // Timer and Divider
            timers->sendingData(address, data);
        } else if (address == 0xFF0F) { // Interrupts
            interrupts->Registers.IF = data;
        } else if (address >= 0xFF10 && address <= 0xFF26) {                   // Audio
        } else if (address >= 0xFF30 && address <= 0xFF3F) {                   // Wave Pattern
        } else if (address >= 0xFF40 && address <= 0xFF4B) {                   // LCD Controller
        } else if (address == 0xFF46) {                                        // OAM DMA Transfer
        } else if ((address >= 0xFF4C && address <= 0xFF4D) && rom->CGBmode) { // KEY0 & KEY1
        } else if (address == 0xFF4F && rom->CGBmode) {                        // VRAM Bank Select
        } else if (address == 0xFF50) {                                        // Boot Rom
            if (data == 0x1) {
                bios->BootBIOS = false;
            }
        } else if ((address >= 0xFF51 && address <= 0xFF55) && rom->CGBmode) { // VRAM DMA
        } else if (address == 0xFF56 && rom->CGBmode) {                        // IR Port
        } else if ((address >= 0xFF68 && address <= 0xFF6B) && rom->CGBmode) { // BG-OBJ Palettes
        } else if (address == 0xFF6C && rom->CGBmode) {                        // Object Priority Mode
        } else if (address == 0xFF70 && rom->CGBmode) {                        // WRAM Bank Select
            wram->sendingData(address, data);
        }
    } else if (address >= 0xFF80 && address <= 0xFFFE) { // High RAM (HRAM)
        HRAM[address & 0x7F] = data;
    } else if (address == 0xFFFF) { // Interrupt Enable Register
        interrupts->Registers.IE = data;
    }
}

uint16_t MMU::readWord(uint16_t address) {
    uint16_t data = readByte(address);
    data |= static_cast<uint16_t>(readByte(address + 1)) << 8;
    return data;
}
void MMU::writeWord(uint16_t address, uint16_t data) {
    writeByte(address, static_cast<uint8_t>(data & 0xFF));
    writeByte(address + 1, static_cast<uint8_t>((data >> 8) & 0xFF));
}