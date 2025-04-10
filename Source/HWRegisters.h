/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Tue Apr 08 2025>  <KGkotzamanidis>
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
#ifndef _HWREGISTERS_H_
#define _HWREGISTERS_H_

#include <cstdint>

struct CPURegisters {
    uint16_t SP;
    uint16_t PC;

    union {
        struct {
            uint8_t F;
            uint8_t A;
        };
        uint16_t AF;
    };

    union {
        struct {
            uint8_t C;
            uint8_t B;
        };
        uint16_t BC;
    };

    union {
        struct {
            uint8_t E;
            uint8_t D;
        };
        uint16_t DE;
    };

    union {
        struct {
            uint8_t L;
            uint8_t H;
        };
        uint16_t HL;
    };

    enum Flags {
        ZERO_FLAG = 1 << 7,
        SUBTRACT_FLAG = 1 << 6,
        HALF_CARRY_FLAG = 1 << 5,
        CARRY_FLAG = 1 << 4
    };
    void setFlag(Flags flag) { F |= flag; }
    void clearFlag(Flags flag) { F &= ~flag; }
    bool checkFlag(Flags flag) const { return (F & flag) != 0; }
};

struct PPURegisters{

};

struct INTERRUPTRegisters{
    uint8_t IF;
    uint8_t IE;
};

struct TIMERRegisters{
    uint8_t TMA;
    uint8_t TAC;
    uint8_t TMC;
    uint8_t TIMA;
};
#endif