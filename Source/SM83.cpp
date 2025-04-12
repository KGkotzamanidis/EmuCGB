/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Wed Apr 09 2025>  <KGkotzamanidis>
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
#include "SM83.h"

// clang-format off
const int CGB_Instruction_Cycle[256] = {
    4, 12, 8, 8, 4, 4, 8, 4, 20, 8, 8, 8, 4, 4, 8, 4,           // 0x00 - 0x0F
    4, 12, 8, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,           // 0x10 - 0x1F
    12, 8, 12, 8, 4, 4, 8, 4, 12, 8, 8, 8, 4, 4, 8, 4,          // 0x20 - 0x2F
    12, 8, 8, 12, 12, 12, 4, 12, 8, 8, 8, 4, 4, 8, 4,           // 0x30 - 0x3F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,             // 0x40 - 0x4F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,             // 0x50 - 0x5F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 4, 4, 8, 4,             // 0x60 - 0x6F
    8, 8, 8, 8, 8, 8, 4, 8, 4, 4, 4, 4, 4, 4, 8, 4,             // 0x70 - 0x7F
    4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4, 8, 4, 4, 4,             // 0x80 - 0x8F
    4, 8, 4, 20, 12, 16, 16, 24, 16, 8, 16, 20, 16, 16, 4,      // 0x90 - 0x9F
    24, 24, 8, 16, 20, 12, 16, 4, 24, 16, 8, 16, 20, 16, 16, 4, // 0xA0 - 0xAF
    24, 4, 8, 16, 12, 12, 8, 4, 4, 16, 8, 16, 16, 4, 16, 4,     // 0xB0 - 0xBF
    12, 8, 4, 4, 16, 8, 16, 16, 4, 16, 4, 4, 4, 8, 16, 12,      // 0xC0 - 0xCF
    12, 8, 4, 4, 16, 8, 16, 12, 8, 16, 4, 4, 4, 8, 16, 12,      // 0xD0 - 0xDF
    8, 16, 4, 4, 16, 8, 16, 12, 8, 16, 4, 4, 4, 8, 16, 12,      // 0xE0 - 0xEF
    8, 16, 4, 4, 16, 8, 16, 12, 8, 16, 4, 4, 4, 8, 16, 12       // 0xF0 - 0xFF
};

const int CGB_PrefixedInstruction_Cycle[256] ={
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCB00 - 0xCB0F
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCB10 - 0xCB1F
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCB20 - 0xCB2F
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCB30 - 0xCB3F
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,           // 0xCB40 - 0xCB4F
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,           // 0xCB50 - 0xCB5F
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,           // 0xCB60 - 0xCB6F
    8, 8, 8, 8, 8, 8, 12, 8, 8, 8, 8, 8, 8, 8, 12, 8,           // 0xCB70 - 0xCB7F
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCB80 - 0xCB8F
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCB90 - 0xCB9F
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCBA0 - 0xCBAF
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCBB0 - 0xCBBF
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCBC0 - 0xCBCF
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCBD0 - 0xCBDF
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8,           // 0xCBE0 - 0xCBEF
    8, 8, 8, 8, 8, 8, 16, 8, 8, 8, 8, 8, 8, 8, 16, 8            // 0xCBF0 - 0xCBFF
};
// clang-format on

SM83::SM83(MMU &mmu) : mmu(&mmu) {
    cycleCount = 0;
}

void SM83::run() {
}

int SM83::step() {
    return cycleCount;
}

uint8_t SM83::n8() {
    return mmu->readByte(Registers.PC++);
}
uint16_t SM83::n16() {
    uint8_t LSB = mmu->readByte(Registers.PC++);
    uint8_t MSB = mmu->readByte(Registers.PC++);
    return (MSB << 8) | LSB;
}

uint16_t SM83::a8() {
    return 0xFF00 + n8();
}

void SM83::executeInstruction() {
    uint8_t opcode = n8();
    cycleCount = CGB_Instruction_Cycle[opcode];

    switch (opcode) {
    default:
        break;
    }
}

void SM83::executePrefixedInstruction() {
    uint8_t opcode = n8();
    cycleCount = CGB_PrefixedInstruction_Cycle[opcode];

    switch (opcode) {
    default:
        break;
    }
}

uint8_t &SM83::getRegisters(uint8_t reg) {
    switch (reg) {
    case 0:
        return Registers.B;
    case 1:
        return Registers.C;
    case 2:
        return Registers.D;
    case 3:
        return Registers.E;
    case 4:
        return Registers.H;
    case 5:
        return Registers.L;
    case 7:
        return Registers.A;
    default:
        break;
    }
}
uint8_t SM83::getRegisterValue(uint8_t reg) {
    return getRegisters(reg);
}

#pragma region 8-Bit Load Instructions
void SM83::LD_nn_n(uint8_t opcode) {
    uint8_t data = n8();

    switch (opcode) {
    case 0x06:
        Registers.B = data;
        break;
    case 0x0E:
        Registers.C = data;
        break;
    case 0x16:
        Registers.D = data;
        break;
    case 0x1E:
        Registers.E = data;
        break;
    case 0x26:
        Registers.H = data;
        break;
    case 0x2E:
        Registers.L = data;
        break;
    case 0x36:
        mmu->writeByte(Registers.HL, data);
        break;
    default:
        break;
    }
}

void SM83::LD_r1_r2(uint8_t opcode) {
    uint8_t r1 = (opcode >> 3) & 0x07;
    uint8_t r2 = opcode & 0x07;

    if (r2 == 6) {
        uint8_t data = mmu->readByte(Registers.HL);

        if (r1 == 6) {
            return;
        }

        LD_r1_r2(getRegisters(r1), data);
    } else {
        uint8_t data = getRegisterValue(r2);

        if (r1 == 6) {
            mmu->writeByte(Registers.HL, data);
        } else {
            LD_r1_r2(getRegisters(r1), data);
        }
    }
}

void SM83::LD_r1_r2(uint8_t &dst, uint8_t &src) {
    dst = src;
}

void SM83::LD_A_n(uint8_t opcode) {
    switch (opcode) {
    case 0x7F:
        Registers.A = Registers.A;
        break;
    case 0x78:
        Registers.A = Registers.B;
        break;
    case 0x79:
        Registers.A = Registers.C;
        break;
    case 0x7A:
        Registers.A = Registers.D;
        break;
    case 0x7B:
        Registers.A = Registers.E;
        break;
    case 0x7C:
        Registers.A = Registers.H;
        break;
    case 0x7D:
        Registers.A = Registers.L;
        break;
    case 0x0A:
        Registers.A = mmu->readByte(Registers.BC);
        break;
    case 0x1A:
        Registers.A = mmu->readByte(Registers.DE);
        break;
    case 0x7E:
        Registers.A = mmu->readByte(Registers.HL);
        break;
    case 0x3E:
        Registers.A = n8();
        break;
    case 0xFA:
        Registers.A = mmu->readByte(n16());
        break;
    default:
        break;
    }
}

void SM83::LD_n_A(uint8_t opcode) {
    switch (opcode) {
    case 0x02:
        mmu->writeByte(Registers.BC, Registers.A);
        break;
    case 0x12:
        mmu->writeByte(Registers.DE, Registers.A);
        break;
    case 0x7F:
        Registers.A = Registers.A;
        break;
    case 0x47:
        Registers.B = Registers.A;
        break;
    case 0x4F:
        Registers.C = Registers.A;
        break;
    case 0x57:
        Registers.D = Registers.A;
        break;
    case 0x5F:
        Registers.E = Registers.A;
        break;
    case 0x67:
        Registers.H = Registers.A;
        break;
    case 0x6F:
        Registers.L = Registers.A;
        break;
    case 0x77:
        mmu->writeByte(Registers.HL, Registers.A);
        break;
    case 0xEA:
        mmu->writeByte(n16(), Registers.A);
        break;
    default:
        break;
    }
}

void SM83::LD_A_C() {
    Registers.A = mmu->readByte(0xFF00 + Registers.C);
}

void SM83::LD_C_A() {
    mmu->writeByte(0xFF00 + Registers.C, Registers.A);
}

void SM83::LDD_A_HL() {
    Registers.A = mmu->readByte(Registers.HL--);
}

void SM83::LDD_HL_A() {
    mmu->writeByte(Registers.HL--, Registers.A);
}

void SM83::LDI_A_HL() {
    Registers.A = mmu->readByte(Registers.HL++);
}

void SM83::LDI_HL_A() {
    mmu->writeByte(Registers.HL++, Registers.A);
}

void SM83::LDH_n_A() {
    mmu->writeByte(0xFF00 + n8(), Registers.A);
}

void SM83::LDH_A_n() {
    Registers.A = mmu->readByte(0xFF00 + n8());
}
#pragma endregion

#pragma region 16-Bit Load Instructions
void SM83::LD_n_nn(uint16_t &dst) {
    dst = n16();
}

void SM83::LD_SP_HL() {
    Registers.SP = Registers.HL;
}

void SM83::LD_HL_SP_n() {
    int8_t i8 = static_cast<int8_t>(n8());
    int32_t result = Registers.SP + i8;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (((Registers.SP ^ i8 ^ (result & 0xFFFF)) & 0x10) != 0) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (((Registers.SP ^ i8 ^ (result & 0xFFFF)) & 0x100) != 0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.HL = static_cast<uint16_t>(result & 0xFFFF);
}

void SM83::LD_nn_SP() {
    mmu->writeWord(n16(), Registers.SP);
}

void SM83::PUSH_nn(uint16_t src) {
    Registers.SP -= 2;
    mmu->writeWord(Registers.SP, src);
}

void SM83::POP_nn(uint16_t &dst) {
    dst = mmu->readWord(Registers.SP);
    Registers.SP += 2;
}
#pragma endregion

#pragma region 8-Bit ALU Instructions
void SM83::ADD_A_n(uint8_t n) {
    uint8_t tmpA = Registers.A;
    uint16_t result = Registers.A + n;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if ((result & 0xFF) == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if (((tmpA & 0xF) + (n & 0xF)) > 0xF) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (result > 0xFF) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }
    Registers.A = static_cast<uint8_t>(result & 0xFF);
}
void SM83::ADC_A_n(uint8_t n) {
    uint8_t carry = Registers.checkFlag(Registers.CARRY_FLAG) ? 1 : 0;
    uint8_t tmpA = Registers.A;
    uint16_t result = Registers.A + n + carry;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if ((result & 0xFF) == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if (((tmpA & 0xF) + (n & 0xF) + carry) > 0xF) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (result > 0xFF) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = static_cast<uint8_t>(result & 0xFF);
}
void SM83::SUB_A_n(uint8_t n) {
    uint8_t tmpA = Registers.A;
    uint16_t result = Registers.A - n;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.setFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if ((result & 0xFF) == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if ((tmpA & 0x0F) < (n & 0x0F)) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (tmpA < n) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = static_cast<uint8_t>(result & 0xFF);
}
void SM83::SBC_A_n(uint8_t n) {}
void SM83::AND_A_n(uint8_t n) {}
void SM83::OR_A_n(uint8_t n) {}
void SM83::XOR_A_n(uint8_t n) {}
void SM83::CP_A_n(uint8_t n) {}
void SM83::INC_n(uint8_t &n) {}
void SM83::DEC_n(uint8_t &n) {}
#pragma endregion
