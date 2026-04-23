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
#include "MBC3.hpp"
#include "NoMBC.hpp"

ROM::ROM() {
    std::printf("[!]ROM Constructor Initiallized\n");
}

ROM *ROM::loadROM(std::string ROMFilePath) {
    std::ifstream ROMFile(ROMFilePath, std::ios::in | std::ios::ate | std::ios::binary);
    ROM *data = nullptr;
    std::vector<uint8_t> *ROMData;
    std::vector<uint8_t> RAMData;
    std::streamsize ROMSize = 0;
    uint8_t *ROMPtr = nullptr;

    std::printf("[?]ROM: Loading ROM from %s\n", ROMFilePath.c_str());

    ROMData = new std::vector<uint8_t>();

    if (ROMFile.is_open()) {
        ROMFile.seekg(0, std::ios::end);
        ROMSize = ROMFile.tellg();
        ROMFile.seekg(0, std::ios::beg);
        ROMData->resize(ROMSize);
        ROMFile.read(reinterpret_cast<char *>(ROMData->data()), ROMSize);
        ROMFile.close();
        ROMPtr = ROMData->data();

        std::printf("[!]ROM: Successfully Load Size:(%zu KiB) @ Address: 0x%p\n", ROMSize, ROMPtr);
    } else {
        std::printf("[X]ROM: File not found.\n");
        ROMSize = 0;
        ROMPtr = nullptr;
    }

    std::printf("-===============ROM Info===============-\n");

    // ROM Title
    char tmpTitle[17];
    std::memcpy(tmpTitle, ROMData->data() + 0x134, 16);
    tmpTitle[16] = '\0';
    std::string ROMTitle = tmpTitle;
    std::printf("ROM Title: %s\n", ROMTitle.c_str());

    // ROM CGB Mode
    std::string CGBMode = (ROMData->data()[0x143] == 0x80) ? "YES" : "NO";
    std::printf("ROM CGBMode: %s\n", CGBMode.c_str());

    // Memory Bank Type
    uint8_t MBCType = (uint8_t)(*ROMData)[0x147];

    // ROM RAM Size
    uint8_t ramCode = (uint8_t)(*ROMData)[0x149];
    const uint32_t ramSizes[7] = {0x0, 0x800, 0x2000, 0x8000, 0x20000, 0x10000, 0x0};
    const char *strRAMSize[7] = {"None", "2KiB", "8KiB", "32KiB", "128KiB", "64KiB", "Other"};
    int lookupIndex = (ramCode <= 5) ? ramCode : 6;
    std::printf("ROM RAM Size: %s (%u bytes)\n", strRAMSize[lookupIndex], ramSizes[lookupIndex]);

    // ROM Save Function
    std::string SaveFilePath = ROMFilePath;
    SaveFilePath.erase(SaveFilePath.find_last_of("."), std::string::npos);
    SaveFilePath.append(".save");

    std::printf("-======================================-\n"); // = 30 + 8

    switch (MBCType) {
    case 0x00:
        data = new NoMBC(ROMData, ROMSize);
        break;
    case 0x13:
        data = new MBC3(ROMData, ROMSize, ramSizes[lookupIndex], false, SaveFilePath);
        break;
    }
    return data;
}