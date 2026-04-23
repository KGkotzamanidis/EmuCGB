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
#ifndef MBC3_HPP
#define MBC3_HPP

#include "ROM.hpp"

class MBC3 : public ROM {
public:
    MBC3(std::vector<uint8_t> *ROMData, std::streamsize ROMSize, uint32_t RAMSize, bool timerPresent, std::string  SaveFilePath);
    ~MBC3();

    uint8_t receivingData(uint16_t address);
    void sendingData(uint16_t address, uint8_t data);

    void saveData();
    void loadData();

private:
    std::vector<uint8_t> *ROMData;
    std::streamsize ROMSize;
    uint32_t RAMSize;
    bool RTC = false;

    std::vector<uint8_t> *RAMData;

    uint16_t currentROMBank = 1;
    uint8_t currentRAMBank = 0;
    bool ramEnable = false;

    struct {
        uint8_t s, m, h, dl, dh;
        uint8_t ls, lm, lh, ldl, ldh;
        std::time_t lastSyncTime = 0;
    } rtc;

    uint8_t lastLatchWrite = 0xFF;

    std::string SaveFilePath;
    bool battery;

    void updateRTC();
};
#endif