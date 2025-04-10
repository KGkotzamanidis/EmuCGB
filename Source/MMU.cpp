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
#include "MMU.h"
MMU::MMU(BIOS &bios, ROM &rom) : bios(&bios), rom(&rom) {
    std::printf("-=MMU class initialized=-\n");
}

uint8_t MMU::readByte(uint16_t address) {
    uint8_t data = 0xFF;

    if (address >= 0x0000 && address <= 0x7FFF) {
        if (address < 0x0100 && bios->isBIOSLoaded) {
            data = bios->receivingData(address);
        } else {
            data = rom->receivingData(address);
        }
    } else if (address >= 0x8000 && address <= 0x9FFF) {
    } else if (address >= 0xA000 && address <= 0xBFFF) {
    } else if (address >= 0xC000 && address <= 0xFDFF) {
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    } else if (address >= 0xFF00 && address <= 0xFF7F) {
        if (address == 0xFF00) {
        } else if (address >= 0xFF01 && address <= 0xFF02) {
        } else if (address >= 0xFF04 && address <= 0xFF07) {
        } else if (address == 0xFF0F) {
        } else if (address >= 0xFF10 && address <= 0xFF26) {
        } else if (address >= 0xFF30 && address <= 0xFF3F) {
        } else if (address >= 0xFF40 && address <= 0xFF4B) {
        } else if (address == 0xFF4F && rom->CGBmode) {
        } else if (address == 0xFF50) {
        } else if ((address >= 0xFF51 && address <= 0xFF55) && rom->CGBmode) {
        } else if ((address >= 0xFF68 && address <= 0xFF6B) && rom->CGBmode) {
        } else if (address == 0xFF70 && rom->CGBmode) {
        }
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
    } else if (address == 0xFFFF) {
        data = interrupts->Registers.IE;
    }

    return data;
}
void MMU::writeByte(uint16_t address, uint8_t data) {
    if (address >= 0x0000 && address <= 0x7FFF) {
    } else if (address >= 0x8000 && address <= 0x9FFF) {
    } else if (address >= 0xA000 && address <= 0xBFFF) {
    } else if (address >= 0xC000 && address <= 0xFDFF) {
    } else if (address >= 0xFE00 && address <= 0xFE9F) {
    } else if (address >= 0xFEA0 && address <= 0xFEFF) {
    } else if (address >= 0xFF00 && address <= 0xFF7F) {
        if (address == 0xFF00) {
        } else if (address >= 0xFF01 && address <= 0xFF02) {
        } else if (address >= 0xFF04 && address <= 0xFF07) {
        } else if (address == 0xFF0F) {
        } else if (address >= 0xFF10 && address <= 0xFF26) {
        } else if (address >= 0xFF30 && address <= 0xFF3F) {
        } else if (address >= 0xFF40 && address <= 0xFF4B) {
        } else if (address == 0xFF4F && rom->CGBmode) {
        } else if (address == 0xFF50) {
        } else if ((address >= 0xFF51 && address <= 0xFF55) && rom->CGBmode) {
        } else if ((address >= 0xFF68 && address <= 0xFF6B) && rom->CGBmode) {
        } else if (address == 0xFF70 && rom->CGBmode) {
        }
    } else if (address >= 0xFF80 && address <= 0xFFFE) {
    } else if (address == 0xFFFF) {
        interrupts->Registers.IE = data;
    }
}

uint16_t MMU::readWord(uint16_t address) {
    uint16_t data = readByte(address);
    data |= static_cast<uint16_t>(readByte(address + 1)) << 8;
    return data;
}
void MMU::writeWord(uint16_t address, uint16_t data) {
    writeByte(address, static_cast<uint8_t>(data & 0xFF));
    writeByte(address + 1, static_cast<uint8_t>((data >> 8) & 0xFF));
}