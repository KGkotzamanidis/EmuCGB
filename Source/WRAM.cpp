/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Wed Apr 16 2025>  <KGkotzamanidis>
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
#include "WRAM.h"
WRAM::WRAM() {
    std::printf("-=WRAM class initialized=-\n");
}

uint8_t WRAM::receivingData(uint16_t address)
{
    uint8_t data = 0xFF;
    if (address >= 0xC000 && address <= 0xFDFF)
    {
        uint16_t wramAddress = (address & 0x1FFF);
        if (wramAddress >= 0x1000)
        {
            wramAddress &= 0xFFF;
            wramAddress |= WRAMBank << 12;
        }
        data = RAM[wramAddress];
    }
    else if (address == 0xFF70)
    {
        data = WRAMBank;
    }
    return data;
}

void WRAM::sendingData(uint16_t address, uint8_t data)
{
    if (address >= 0xC000 && address <= 0xFDFF)
    {
        uint16_t wramAddress = (address & 0x1FFF);
        if (wramAddress >= 0x1000)
        {
            wramAddress &= 0xFFF;
            wramAddress |= WRAMBank << 1;
        }
        RAM[wramAddress] = data;
    }
    else if (address == 0xFF70)
    {
        WRAMBank = data & 0x7;
        if (WRAMBank == 0)
        {
            WRAMBank = 1;
        }
    }
}