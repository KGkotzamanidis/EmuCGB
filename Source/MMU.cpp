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
#include "MMU.hpp"
#include "PPU.hpp"

MMU::MMU(BIOS &bios, ROM *rom, Interrupts &interrupts, Joypad &joypad, Timers &timers, WRAM &wram, bool CGBMode) : bios(&bios), rom(rom), interrupts(&interrupts), joypad(&joypad), timers(&timers), wram(&wram), CGBMode(CGBMode) {
    std::printf("[!]MMU Constructor Initiallized\n");
}

uint8_t MMU::readByte(uint16_t address) {
    uint8_t data = 0xFF;

    if (bios->BOOT_BIOS) {
        if (address <= 0x00FF) {
            return bios->receivingData(address);
        }

        if (CGBMode && (address >= 0x0200 && address <= 0x08FF)) {
            return bios->receivingData(address);
        }
    }

    if (address >= 0x0000 && address <= 0x7FFF) {
        data = rom->receivingData(address);
    } else if (address >= 0x8000 && address <= 0x9FFF) { // VRAM → PPU
        data = ppu->readVRAM(address);
    } else if (address >= 0xA000 && address <= 0xBFFF) { // External RAM
        data = rom->receivingData(address);
    } else if (address >= 0xC000 && address <= 0xFDFF) { // WRAM + Echo
        data = wram->receivingData(address);
    } else if (address >= 0xFE00 && address <= 0xFE9F) { // OAM → PPU
        data = ppu->readOAM(address);
    } else if (address >= 0xFEA0 && address <= 0xFEFF) { // Unusable — open bus
        // FIX: upper bound was 0xFF70, missing 0xFF71-0xFF7F
    } else if (address >= 0xFF00 && address <= 0xFF7F) {     // I/O Registers
        if (address == 0xFF00) {                             // Joypad
        } else if (address >= 0xFF01 && address <= 0xFF02) { // Serial
        } else if (address >= 0xFF04 && address <= 0xFF07) { // Timers
            data = timers->receivingData(address);
        } else if (address == 0xFF0F) {                      // IF
            data = interrupts->Registers.IF | 0xE0;          // bits 7-5 always 1
        } else if (address >= 0xFF10 && address <= 0xFF26) { // APU
        } else if (address >= 0xFF30 && address <= 0xFF3F) { // Wave RAM
        } else if (address >= 0xFF40 && address <= 0xFF4B) { // LCD registers → PPU
            data = ppu->readIO(address);
        } else if (address == 0xFF46) {                                   // OAM DMA — write-only
            data = 0xFF;                                                  // reads return open bus
        } else if ((address >= 0xFF4C && address <= 0xFF4D) && CGBMode) { // KEY0/KEY1
            data = (address == 0xFF4C) ? KEY_0 : KEY_1;
        } else if (address == 0xFF4F && CGBMode) { // VBK → PPU
            data = ppu->readIO(address);
        } else if (address == 0xFF50) { // Boot ROM mapped flag
            data = bios->BOOT_BIOS ? 0x00 : 0x01;
        } else if ((address >= 0xFF51 && address <= 0xFF55) && CGBMode) { // HDMA
            data = 0xFF;                                                  // stub — HDMA not yet implemented
        } else if (address == 0xFF56 && CGBMode) {                        // IR port
            data = 0xFF;                                                  // stub
        } else if ((address >= 0xFF68 && address <= 0xFF6B) && CGBMode) { // CGB palettes → PPU
            data = ppu->readIO(address);
        } else if (address == 0xFF6C && CGBMode) { // Object priority mode
            data = 0xFF;                           // stub
        } else if (address == 0xFF70 && CGBMode) { // SVBK — WRAM bank
            data = wram->receivingData(address);
        }
        // FIX: HRAM index was (address & 0x7F) — wrong for 0xFF80+
        //      0xFF80 & 0x7F = 0x00, 0xFFFE & 0x7F = 0x7E — coincidentally correct
        //      but address - 0xFF80 is explicit and safer
    } else if (address >= 0xFF80 && address <= 0xFFFE) { // HRAM
        data = HRAM[address - 0xFF80];
    } else if (address == 0xFFFF) { // IE
        data = interrupts->Registers.IE;
    }

    return data;
}
void MMU::writeByte(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x7FFF) { // ROM — MBC writes
        rom->sendingData(address, data);
    } else if (address >= 0x8000 && address <= 0x9FFF) { // VRAM → PPU
        ppu->writeVRAM(address, data);
    } else if (address >= 0xA000 && address <= 0xBFFF) { // External RAM
        rom->sendingData(address, data);
    } else if (address >= 0xC000 && address <= 0xFDFF) { // WRAM + Echo
        wram->sendingData(address, data);
    } else if (address >= 0xFE00 && address <= 0xFE9F) { // OAM → PPU
        ppu->writeOAM(address, data);
    } else if (address >= 0xFEA0 && address <= 0xFEFF) { // Unusable — ignore
        // FIX: upper bound was 0xFF70, missing 0xFF71-0xFF7F
    } else if (address >= 0xFF00 && address <= 0xFF7F) {     // I/O Registers
        if (address == 0xFF00) {                             // Joypad
        } else if (address >= 0xFF01 && address <= 0xFF02) { // Serial
        } else if (address >= 0xFF04 && address <= 0xFF07) { // Timers
            timers->sendingData(address, data);
        } else if (address == 0xFF0F) {                      // IF
            interrupts->Registers.IF = data & 0x1F;          // only bits 4-0 are writable
        } else if (address >= 0xFF10 && address <= 0xFF26) { // APU
        } else if (address >= 0xFF30 && address <= 0xFF3F) { // Wave RAM
        } else if (address >= 0xFF40 && address <= 0xFF4B) { // LCD registers → PPU
            ppu->writeIO(address, data);
        } else if (address == 0xFF46) { // OAM DMA — trigger transfer
            // The CPU writes the source page (e.g. 0xC0 = copy from 0xC000).
            // startDMA reads 160 bytes via the MMU readByte callback into OAM.
            ppu->startDMA(data, [](uint16_t addr, void *ctx) { return reinterpret_cast<MMU *>(ctx)->readByte(addr); }, this);
        } else if ((address >= 0xFF4C && address <= 0xFF4D) && CGBMode) { // KEY0/KEY1
        } else if (address == 0xFF4F && CGBMode) {                        // VBK → PPU
            ppu->writeIO(address, data);
        } else if (address == 0xFF50) { // Boot ROM disable
            if (data & 0x01)
                bios->BOOT_BIOS = false;
        } else if ((address >= 0xFF51 && address <= 0xFF55) && CGBMode) { // HDMA
        } else if (address == 0xFF56 && CGBMode) {                        // IR port
        } else if ((address >= 0xFF68 && address <= 0xFF6B) && CGBMode) { // CGB palettes → PPU
            ppu->writeIO(address, data);
        } else if (address == 0xFF6C && CGBMode) { // Object priority mode
        } else if (address == 0xFF70 && CGBMode) { // SVBK — WRAM bank
            wram->sendingData(address, data);
        }
        // FIX: HRAM index address - 0xFF80 instead of address & 0x7F
    } else if (address >= 0xFF80 && address <= 0xFFFE) { // HRAM
        HRAM[address - 0xFF80] = data;
    } else if (address == 0xFFFF) { // IE
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

void MMU::connectPPU(PPU &ppu) {
    this->ppu = &ppu;
}

bool MMU::isCBGMode() {
    return CGBMode;
}