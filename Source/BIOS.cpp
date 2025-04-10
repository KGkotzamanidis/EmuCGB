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
#include "BIOS.h"
BIOS::BIOS() {
    BIOSData.clear();
    
    BIOSSize = 0;
    isBIOSLoaded = false;
    BIOSPtr = nullptr;

    std::printf("-=BIOS class initialized=-\n");
}

void BIOS::loadBIOS(std::string BIOSFilePath) {
    std::ifstream BIOSReader(BIOSFilePath, std::ios::in | std::ios::ate | std::ios::binary);

    std::printf("\nTry to load BIOS from %s\n", BIOSFilePath.c_str());

    if (BIOSReader.is_open()) {
        BIOSReader.seekg(0, std::ios::end);
        BIOSSize = BIOSReader.tellg();
        BIOSReader.seekg(0, std::ios::beg);
        BIOSData.resize(BIOSSize);
        BIOSReader.read(reinterpret_cast<char *>(BIOSData.data()), BIOSSize);
        BIOSReader.close();
        isBIOSLoaded = true;
        BIOSPtr = BIOSData.data();
        std::printf("BIOS file readed successfully\nSize:(%zu KiB) @ Address: 0x%p\n", BIOSSize, BIOSPtr);
    } else {
        std::printf("BIOS file not found\n");
        isBIOSLoaded = false;
        BIOSSize = 0;
        BIOSPtr = nullptr;
        return;
    }
}

uint8_t BIOS::receivingData(uint16_t address){
    return BIOSData[address];
}