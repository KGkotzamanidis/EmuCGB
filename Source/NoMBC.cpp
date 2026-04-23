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
#include "NoMBC.hpp"

NoMBC::NoMBC(std::vector<uint8_t> *ROMData, std::streamsize ROMSize) : ROMData(ROMData), ROMSize(ROMSize) {
    std::printf("[!]NoMBC Constructor Initiallized\n");
}

NoMBC::~NoMBC() {
    this->ROMData->clear();
}
uint8_t NoMBC::receivingData(uint16_t address) {
    return (*ROMData)[address & 0x7FFF];
}