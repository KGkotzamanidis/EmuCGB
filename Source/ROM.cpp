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
#include "ROM.hpp"
ROM::ROM() {
    std::printf("-=ROM class initialized=-\n");
}

uint8_t ROM::receivingData(uint16_t address) {
    if (address < ROMData.size())
        return ROMData[address];
    return 0xFF;
}

void ROM::sendingData(uint16_t address, uint8_t data) {
}

void ROM::loadROM(std::string ROMFilePath) {
    std::ifstream ROMFile(ROMFilePath, std::ios::in | std::ios::ate | std::ios::binary);

    std::printf("\nTry to load ROM from %s\n", ROMFilePath.c_str());

    if (ROMFile.is_open()) {
        ROMFile.seekg(0, std::ios::end);
        ROMSize = ROMFile.tellg();
        ROMFile.seekg(0, std::ios::beg);
        ROMData.resize(ROMSize);
        ROMFile.read(reinterpret_cast<char *>(ROMData.data()), ROMSize);
        ROMFile.close();
        ROMPtr = ROMData.data();
        std::printf("ROM file readed successfully\nSize:(%zu KiB)@ Address: 0x%p\n", ROMSize, ROMPtr);
    } else {
        std::printf("ROM file not found\n");
        ROMSize = 0;
        ROMPtr = nullptr;
        return;
    }
}