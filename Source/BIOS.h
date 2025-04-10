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
#ifndef _BIOS_H_
#define _BIOS_H_

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

class BIOS {
public:
    BIOS();

    std::vector<uint8_t> BIOSData;

    bool isBIOSLoaded;

    void loadBIOS(std::string BIOSFilePath);

    uint8_t receivingData(uint16_t address);

private:
    std::streamsize BIOSSize;
    uint8_t *BIOSPtr;
};
#endif