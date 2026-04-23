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
#include "BIOS.hpp"

BIOS::BIOS() {
    BOOT_BIOS = false;
    isGB_BIOSUP = false;
    isCGB_BIOSUP = false;

    std::printf("[!]BIOS Constructor Initiallized\n");
}

void BIOS::loadGB_BIOS(std::string BIOSFilePath) {
    std::ifstream BIOSReader(BIOSFilePath, std::ios::in | std::ios::ate | std::ios::binary);

    std::printf("[?]BIOS: Loading GB BIOS from %s\n", BIOSFilePath.c_str());

    if (BIOSReader.is_open()) {
        BIOSReader.seekg(0, std::ios::end);
        GB_BIOSize = BIOSReader.tellg();
        BIOSReader.seekg(0, std::ios::beg);
        GB_BIOSData.resize(GB_BIOSize);
        BIOSReader.read(reinterpret_cast<char *>(GB_BIOSData.data()), GB_BIOSize);
        BIOSReader.close();
        isGB_BIOSUP = true;
        BOOT_BIOS = true;
        GB_BIOSPtr = GB_BIOSData.data();
        std::printf("[!]BIOS: Successfully Load Size:(%zu KiB) @ Address: 0x%p\n", GB_BIOSize, GB_BIOSPtr);
    } else {
        std::printf("[X]BIOS: File not found.\n");
        isGB_BIOSUP = false;
        BOOT_BIOS = false;
        GB_BIOSize = 0;
        GB_BIOSPtr = nullptr;
        return;
    }
}

void BIOS::loadCGB_BIOS(std::string BIOSFilePath) {
    std::ifstream BIOSReader(BIOSFilePath, std::ios::in | std::ios::ate | std::ios::binary);

    std::printf("[?]BIOS: Loading CGB BIOS from %s\n", BIOSFilePath.c_str());

    if (BIOSReader.is_open()) {
        BIOSReader.seekg(0, std::ios::end);
        CGB_BIOSize = BIOSReader.tellg();
        BIOSReader.seekg(0, std::ios::beg);
        CGB_BIOSData.resize(CGB_BIOSize);
        BIOSReader.read(reinterpret_cast<char *>(CGB_BIOSData.data()), CGB_BIOSize);
        BIOSReader.close();
        isCGB_BIOSUP = true;
        BOOT_BIOS = true;
        CGB_BIOSPtr = CGB_BIOSData.data();
        std::printf("[!]BIOS: Successfully Load Size:(%zu KiB) @ Address: 0x%p\n", CGB_BIOSize, CGB_BIOSPtr);
    } else {
        std::printf("[X]BIOS: File not found.\n");
        isGB_BIOSUP = false;
        BOOT_BIOS = false;
        GB_BIOSize = 0;
        GB_BIOSPtr = nullptr;
        return;
    }
}

uint8_t BIOS::receivingData(uint16_t address) {
    if (address <= 0x00FF) {
        return GB_BIOSData[address];
    }

    if (isCGB_BIOSUP && (address >= 0x0200 && address <= 0x08FF)) {
        return CGB_BIOSData[address];
    }
}