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
#ifndef TIMERS_H
#define TIMERS_H

#include "EmulationUtils.h"
#include "Interrupts.h"

class Timers {
public:
    Timers(Interrupts &interrupts);

    uint8_t receivingData(uint16_t address);
    void sendingData(uint16_t address, uint8_t data);

    void updateTimers(int lastCycleCount);

private:
    Interrupts *interrupts = nullptr;

    uint8_t DIV = 0x00, TIMA = 0x00, TMA = 0x00, TAC = 0x00;

    int DIVCycleCount = 0, TIMACycleCount = 0;
    int clockRate(int code);
};
#endif