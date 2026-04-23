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
#include "MBC3.hpp"

MBC3::MBC3(std::vector<uint8_t> *ROMData, std::streamsize ROMSize, uint32_t RAMSize, bool timerPresent, std::string SaveFilePath) : ROMData(ROMData), ROMSize(ROMSize), RAMSize(RAMSize), RTC(timerPresent), SaveFilePath(SaveFilePath) {

    rtc = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, std::time(nullptr)};

    if (RAMSize > 0) {
        this->RAMData = new std::vector<uint8_t>(RAMSize, 0x00);
    } else {
        this->RAMData = nullptr;
    }
    loadData();
    std::printf("[!]MBC3 Constructor Initiallized\n");
}

MBC3::~MBC3() {
    if (this->ROMData != nullptr) {
        delete this->ROMData;
        this->ROMData = nullptr;
    }

    if (this->RAMData != nullptr) {
        delete this->RAMData;
        this->RAMData = nullptr;
    }

    this->ROMSize = 0;
    this->RAMSize = 0;
}

uint8_t MBC3::receivingData(uint16_t address) {
    if (address <= 0x7FFF) {
        uint32_t bank = (address < 0x4000) ? 0 : currentROMBank;
        uint32_t realAddress = (bank * 0x4000) + (address & 0x3FFF);
        return (*ROMData)[realAddress % ROMData->size()];
    }

    if (address >= 0xA000 && address <= 0xBFFF && ramEnable) {
        if (currentRAMBank <= 0x03 && RAMData) {
            return (*RAMData)[(currentRAMBank * 0x2000) + (address & 0x1FFF)];
        } else if (RTC && currentRAMBank >= 0x08 && currentRAMBank <= 0x0C) {
            switch (currentRAMBank) {
            case 0x08:
                return rtc.ls;
            case 0x09:
                return rtc.lm;
            case 0x0A:
                return rtc.lh;
            case 0x0B:
                return rtc.ldl;
            case 0x0C:
                return rtc.ldh;
            }
        }
    }
    return 0xFF;
}

void MBC3::sendingData(uint16_t address, uint8_t data) {
    if (address <= 0x1FFF) {
        ramEnable = ((data & 0x0F) == 0x0A);
    } else if (address <= 0x3FFF) {
        currentROMBank = (data == 0) ? 1 : (data & 0x7F);
    } else if (address <= 0x5FFF) {
        currentRAMBank = data;
    } else if (address <= 0x7FFF) {
        if (RTC && lastLatchWrite == 0x00 && data == 0x01) {
            updateRTC();

            rtc.ls = rtc.s;
            rtc.lm = rtc.m;
            rtc.lh = rtc.h;
            rtc.ldl = rtc.dl;
            rtc.ldh = rtc.dh;
        }
        lastLatchWrite = data;
    } else if (address >= 0xA000 && address <= 0xBFFF && ramEnable) {
        if (currentRAMBank <= 0x03 && RAMData) {
            (*RAMData)[(currentRAMBank * 0x2000) + (address & 0x1FFF)] = data;
        } else if (RTC && currentRAMBank >= 0x08 && currentRAMBank <= 0x0C) {
            updateRTC();

            switch (currentRAMBank) {
            case 0x08:
                rtc.s = data & 0x3F;
                break;
            case 0x09:
                rtc.m = data & 0x3F;
                break;
            case 0x0A:
                rtc.h = data & 0x1F;
                break;
            case 0x0B:
                rtc.dl = data;
                break;
            case 0x0C:
                rtc.dh = data;
                break;
            }
        }
    }
}

void MBC3::saveData() {
    std::printf("[?]MBC3: Saving Memory Data.\n");
    if (this->SaveFilePath.empty()) {
        std::printf("[X]MBC3: Error, the savepath is null\n");
        return;
    }

    if (RAMData) {
        std::ofstream saveFile(this->SaveFilePath, std::ios::binary);

        if (saveFile.is_open()) {
            saveFile.write(reinterpret_cast<char *>(RAMData->data()), RAMData->size());
            saveFile.close();
        }
    }

    if (RTC) {
        updateRTC();
        std::string rtcPath = this->SaveFilePath.substr(0, this->SaveFilePath.find_last_of('.')) + ".rtc";
        std::ofstream rtcFile(rtcPath, std::ios::binary);

        if (rtcFile.is_open()) {
            rtcFile.write(reinterpret_cast<char *>(&rtc.s), 5);
            rtcFile.write(reinterpret_cast<char *>(&rtc.lastSyncTime), sizeof(std::time_t));
        }
    }
    std::printf("[!]MBC3: Successful Saved Memory Data.\n");
}

void MBC3::loadData() {
    std::printf("[?]MBC3: Loading Memory Data.\n");
    if (this->SaveFilePath.empty()) {
        std::printf("[X]MBC3: Error, the savepath is null\n");
        return;
    }

    std::ifstream loadFile(this->SaveFilePath, std::ios::binary);

    if (loadFile.is_open() && RAMData) {
        loadFile.read(reinterpret_cast<char *>(RAMData->data()), RAMData->size());
    }

    if (RTC) {
        std::string rtcPath = this->SaveFilePath.substr(0, this->SaveFilePath.find_last_of('.')) + ".rtc";
        std::ifstream rtcFile(rtcPath, std::ios::binary);

        if (rtcFile.is_open()) {
            rtcFile.read(reinterpret_cast<char *>(&rtc.s), 5);
            rtcFile.read(reinterpret_cast<char *>(&rtc.lastSyncTime), sizeof(std::time_t));
            updateRTC();
        }
    }
    std::printf("[!]MBC3: Successful Loaded Memory Data.\n");
}

void MBC3::updateRTC() {
    if (!RTC || (rtc.dh & 0x40)) {
        return;
    }

    std::time_t now = std::time(nullptr);
    double diff = std::difftime(now, rtc.lastSyncTime);

    if (diff <= 0) {
        return;
    }

    rtc.lastSyncTime = now;
    uint32_t secToAdd = static_cast<uint32_t>(diff);

    uint32_t s = rtc.s + secToAdd;
    rtc.s = s % 60;
    uint32_t m = rtc.m + (s / 60);
    rtc.m = m % 60;
    uint32_t h = rtc.h + (m / 60);
    rtc.h = h % 24;

    uint16_t days = ((rtc.dh & 0x01) << 8) | rtc.dl;
    uint32_t d = days + (h / 24);
    rtc.dl = d & 0xFF;
    rtc.dh = (rtc.dh & 0xFE) | ((d >> 8) & 0x01);

    if (d > 511) {
        rtc.dh |= 0x80;
    }
}