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
        /*Z*/
        ZERO_FLAG = 1 << 7,
        /*N*/
        SUBTRACT_FLAG = 1 << 6,
        /*H*/
        HALF_CARRY_FLAG = 1 << 5,
        /*C*/
        CARRY_FLAG = 1 << 4
    };
    void setFlag(Flags flag) { F |= flag; }
    void clearFlag(Flags flag) { F &= ~flag; }
    bool checkFlag(Flags flag) const { return (F & flag) != 0; }

    bool isHalted = false;
    bool IME = false;
    bool IME_pending = false;
    bool haltBug = false;
    bool DoubleSpeed = false;
};

struct PPURegisters {
    uint8_t LCDC = 0x91;
    uint8_t STAT = 0x00;
    uint8_t SCY = 0x00;
    uint8_t SCX = 0x00;
    uint8_t LY = 0x00;
    uint8_t LYC = 0x00;
    uint8_t BGP = 0xFC;
    uint8_t OBP0 = 0xFF;
    uint8_t OBP1 = 0xFF;
    uint8_t WY = 0x00;
    uint8_t WX = 0x00;
    uint8_t VBK = 0x00;
    uint8_t BGPI = 0x00;
    uint8_t OBPI = 0x00;
    uint8_t BGPD[64] = {};
    uint8_t OBPD[64] = {};

    bool lcdEnabled() const { return (LCDC >> 7) & 1; }
    bool winMapHigh() const { return (LCDC >> 6) & 1; }
    bool winEnabled() const { return (LCDC >> 5) & 1; }
    bool tilesSigned() const { return !((LCDC >> 4) & 1); }
    bool bgMapHigh() const { return (LCDC >> 3) & 1; }
    bool spriteTall() const { return (LCDC >> 2) & 1; }
    bool spritesEnabled() const { return (LCDC >> 1) & 1; }
    bool bgWinEnabled() const { return (LCDC >> 0) & 1; }

    uint8_t getMode() const { return STAT & 0x03; }
    bool isCoincidence() const { return (STAT >> 2) & 1; }

    void setMode(uint8_t mode) { STAT = (STAT & 0xFC) | (mode & 0x03); }
    void setCoincidence(bool v) {
        if (v)
            STAT |= 0x04;
        else
            STAT &= ~0x04;
    }
};

struct INTERRUPTRegisters {
    uint8_t IF;
    uint8_t IE;
};

struct TIMERRegisters {
    uint8_t TMA;
    uint8_t TAC;
    uint8_t TMC;
    uint8_t TIMA;
};

#endif