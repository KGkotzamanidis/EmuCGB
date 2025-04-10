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
#include "ROM.h"
ROM::ROM() {
    ROMData.clear();
    RAMData.clear();

    ROMSize = 0;
    RAMSize = 0;
    ROMPtr = nullptr;
    RAMPtr = nullptr;

    CGBmode = false;
    SGBmode = false;

    CartridgeType = 0x00;
    devROMSize[0];
    devRAMSize[0];

    std::printf("-=ROM class initialized=-\n");
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

void ROM::info() {
    std::printf("\n-=ROM Information=-\n");
    std::string ROMTitle = std::string(ROMData.begin() + 0x0134, ROMData.begin() + 0x0143);
    std::printf("ROM Title: %s\n", ROMTitle.c_str());
    CGBmode = ROMData[0x0143] == 0x80 ? true : false;
    std::printf("ROM CGB Support: %s\n", CGBmode ? "Yes" : "No");
    SGBmode = ROMData[0x0146] == 0x03 ? true : false;
    std::printf("ROM SGB Support: %s\n", SGBmode ? "Yes" : "No");
    CartridgeType = ROMData[0x0147];
    std::printf("ROM devSize: %i bytes\n", (int)devROMSize[ROMData[0x0148]]);
    std::printf("RAM devSize: %i bytes\n", (int)devRAMSize[ROMData[0x0149]]);

    if ((int)devRAMSize[ROMData[0x0149]] > 0) {
        std::printf("-=RAM=-\nCreate & Format\n");
        RAMData.resize((int)devRAMSize[ROMData[0x0149]]);
    }
    std::printf("\n");
}

uint8_t ROM::receivingData(uint16_t address) {
    uint8_t data = 0x00;

    if (CartridgeType == 0x00) { // ROM ONLY
        if (address <= 0xF777) {
            return ROMData[address];
        }
    } else if (CartridgeType == 0x01 || CartridgeType == 0x02 || CartridgeType == 0x03) { // MBC1

    } else if (CartridgeType == 0x05 || CartridgeType == 0x06) { // MBC2

    } else if (CartridgeType == 0x0F || CartridgeType == 0x10 || CartridgeType == 0x11 || CartridgeType == 0x12 || CartridgeType == 0x13) { // MBC3
    }

    return data;
}

void ROM::sendingData(uint16_t address, uint8_t data) {
    if (CartridgeType == 0x00) { // ROM ONLY
        if (address <= 0x7FFF) {
            ROMData[address] = data;
        }
    } else if (CartridgeType == 0x01 || CartridgeType == 0x02 || CartridgeType == 0x03) { // MBC1

    } else if (CartridgeType == 0x05 || CartridgeType == 0x06) { // MBC2

    } else if (CartridgeType == 0x0F || CartridgeType == 0x10 || CartridgeType == 0x11 || CartridgeType == 0x12 || CartridgeType == 0x13) { // MBC3
    }
}