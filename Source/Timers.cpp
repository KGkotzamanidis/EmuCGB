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
#include "Timers.hpp"

Timers::Timers(Interrupts &interrupts) : interrupts(&interrupts) {
    std::printf("-=Timers class initialized=-\n");
}

uint8_t Timers::receivingData(uint16_t address) {
    uint8_t data = 0x00;

    switch (address) {
    case DIVaddress:
        data = DIV;
        break;
    case TIMAaddress:
        data = TIMA;
        break;
    case TMAaddress:
        data = TMA;
        break;
    case TACaddress:
        data = TAC;
        break;
    }

    return data;
}

void Timers::sendingData(uint16_t address, uint8_t data) {
    switch (address) {
    case DIVaddress:
        DIV = 0;
        break;
    case TIMAaddress:
        TIMA = data;
        break;
    case TMAaddress:
        TMA = data;
        break;
    case TACaddress:
        TAC = data;
        break;
    }
}

void Timers::updateTimers(int lastCycleCount) {
    DIVCycleCount += lastCycleCount;
    if (DIVCycleCount >= 256) {
        DIVCycleCount -= 256;
        DIV++;
    }

    if ((TAC & 0x4) != 0) {
        TIMACycleCount += lastCycleCount;
        int clockRateNum = clockRate(TAC & 0x3);

        while (TIMACycleCount >= clockRateNum) {
            TIMACycleCount -= clockRateNum;
            TIMA++;

            if (TIMA == 0) {
                TIMA = TMA;
                interrupts->Registers.IF |= timerOverflow;
            }
        }
    }
}

int Timers::clockRate(int code) {
    int data = 0;

    switch (code) {
    case 0:
        data = 1024;
        break;
    case 1:
        data = 16;
        break;
    case 2:
        data = 64;
        break;
    case 3:
        data = 256;
        break;
    }

    return data;
}