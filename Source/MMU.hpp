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
#ifndef _MMU_H_
#define _MMU_H_

#include "BIOS.hpp"
#include "EmulationUtils.hpp"
#include "Joypad.hpp"
#include "ROM.hpp"
#include "Timers.hpp"
#include "WRAM.hpp"

class PPU;
class MMU {
public:
    MMU(BIOS &bios, ROM &rom, Interrupts &interrupts, Joypad &joypad, Timers &timers, WRAM &wram);
    /*
     * 8Bit read/write functions
     * @param address The address to read from or write to.
     * @param data The data to write (only used in write functions).
     * @return The data read from the address (only used in read functions).
     * @note The readByte function reads one byte from the address.
     */
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t data);

    /*
     * 16Bit read/write functions
     * @param address The address to read from or write to.
     * @param data The data to write (only used in write functions).
     * @return The data read from the address (only used in read functions).
     * @note The readWord function reads two bytes from the address and combines them into a 16-bit word.
     */
    uint16_t readWord(uint16_t address);
    void writeWord(uint16_t address, uint16_t data);

    void connectPPU(PPU &ppu);

private:
    BIOS *bios = nullptr;
    ROM *rom = nullptr;
    Interrupts *interrupts = nullptr;
    Joypad *joypad = nullptr;
    Timers *timers = nullptr;
    WRAM *wram = nullptr;
    PPU *ppu = nullptr;

    std::vector<uint8_t> HRAM = std::vector<uint8_t>(0x7F, 0);
    uint8_t KEY_0 = 0x04;
    uint8_t KEY_1 = 0x7E;
};
#endif