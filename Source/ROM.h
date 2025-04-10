/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Sun Apr 06 2025>  <KGkotzamanidis>
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
#ifndef _ROM_H_
#define _ROM_H_

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

class ROM {
public:
    ROM();

    void loadROM(std::string ROMFilePath);
    void info(void);

    uint8_t receivingData(uint16_t address);
    void sendingData(uint16_t address, uint8_t data);

    bool CGBmode, SGBmode;

private:
    std::vector<uint8_t> ROMData, RAMData;
    std::streamsize ROMSize, RAMSize;
    uint8_t *ROMPtr, *RAMPtr;

    uint8_t CartridgeType;

    int devROMSize[9] = {
        32 * 1024,       // 32KiB
        64 * 1024,       // 64KiB
        128 * 1024,      // 128KiB
        256 * 1024,      // 256KiB
        512 * 1024,      // 512KiB
        1 * 1024 * 1024, // 1MiB
        2 * 1024 * 1024, // 2MiB
        4 * 1024 * 1024, // 4MiB
        8 * 1024 * 1024, // 8MiB
    };

    int devRAMSize[6] = {
        0,
        0,
        8 * 1024,   // 8KiB
        32 * 1024,  // 32KiB
        128 * 1024, // 128KiB
        64 * 1024}; // 64KiB
};
#endif