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
// Cycle counts are the *taken* cycle counts (branch-taken / memory-access path).
// For conditional instructions (JP cc, CALL cc, RET cc) that have two timings,
// the value here is the longer (taken) path; the not-taken path is handled in
// the branch helpers by returning the delta.
const int CGB_Instruction_Cycle[256] = {
    //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
        4, 12,  8,  8,  4,  4,  8,  4, 20,  8,  8,  8,  4,  4,  8,  4,  // 0x00 - 0x0F
        4, 12,  8,  8,  4,  4,  8,  4, 12,  8,  8,  8,  4,  4,  8,  4,  // 0x10 - 0x1F
       12,  8, 12,  8,  4,  4,  8,  4, 12,  8, 12,  8,  4,  4,  8,  4,  // 0x20 - 0x2F
       12,  8,  8,  8,  8,  8, 12,  4, 12,  8,  8,  8,  4,  4,  8,  4,  // 0x30 - 0x3F
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x40 - 0x4F
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x50 - 0x5F
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x60 - 0x6F
        8,  8,  8,  8,  8,  8,  4,  8,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x70 - 0x7F
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x80 - 0x8F
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0x90 - 0x9F
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0xA0 - 0xAF
        4,  4,  4,  4,  4,  4,  8,  4,  4,  4,  4,  4,  4,  4,  8,  4,  // 0xB0 - 0xBF
        8, 12, 12, 16, 12, 16,  8, 16,  8, 16, 12,  4, 12, 24,  8, 16,  // 0xC0 - 0xCF
        8, 12, 12,  4, 12, 16,  8, 16,  8, 16, 12,  4, 12,  4,  8, 16,  // 0xD0 - 0xDF
       12, 12,  8,  4,  4, 16,  8, 16, 16,  4, 16,  4,  4,  4,  8, 16,  // 0xE0 - 0xEF
       12, 12,  8,  4,  4, 16,  8, 16, 12,  8, 16,  4,  4,  4,  8, 16   // 0xF0 - 0xFF
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
    std::printf("-=SM83 class initialized=-\n");
}

int SM83::step() {
    if (Registers.isHalted) {
        return 4; // CPU is stalled but the clock still ticks
    }

    executeInstruction();

    // EI enables interrupts one instruction later, not immediately
    if (Registers.IME_pending) {
        Registers.IME = true;
        Registers.IME_pending = false;
    }

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

    // -------------------------------------------------------------------------
    // 0x00  NOP
    case 0x00: NOP(); break;

    // -------------------------------------------------------------------------
    // 0x01  LD BC,nn
    case 0x01: LD_n_nn(Registers.BC); break;
    // 0x11  LD DE,nn
    case 0x11: LD_n_nn(Registers.DE); break;
    // 0x21  LD HL,nn
    case 0x21: LD_n_nn(Registers.HL); break;
    // 0x31  LD SP,nn
    case 0x31: LD_n_nn(Registers.SP); break;

    // -------------------------------------------------------------------------
    // 0x02  LD (BC),A
    case 0x02: mmu->writeByte(Registers.BC, Registers.A); break;
    // 0x12  LD (DE),A
    case 0x12: mmu->writeByte(Registers.DE, Registers.A); break;
    // 0x22  LD (HL+),A  / LDI (HL),A
    case 0x22: LDI_HL_A(); break;
    // 0x32  LD (HL-),A  / LDD (HL),A
    case 0x32: LDD_HL_A(); break;

    // -------------------------------------------------------------------------
    // 0x03  INC BC
    case 0x03: INC_nn(Registers.BC); break;
    // 0x13  INC DE
    case 0x13: INC_nn(Registers.DE); break;
    // 0x23  INC HL
    case 0x23: INC_nn(Registers.HL); break;
    // 0x33  INC SP
    case 0x33: INC_nn(Registers.SP); break;

    // -------------------------------------------------------------------------
    // 0x04  INC B
    case 0x04: INC_n(Registers.B); break;
    // 0x0C  INC C
    case 0x0C: INC_n(Registers.C); break;
    // 0x14  INC D
    case 0x14: INC_n(Registers.D); break;
    // 0x1C  INC E
    case 0x1C: INC_n(Registers.E); break;
    // 0x24  INC H
    case 0x24: INC_n(Registers.H); break;
    // 0x2C  INC L
    case 0x2C: INC_n(Registers.L); break;
    // 0x34  INC (HL)
    case 0x34: {
        uint8_t val = mmu->readByte(Registers.HL);
        INC_n(val);
        mmu->writeByte(Registers.HL, val);
        break;
    }
    // 0x3C  INC A
    case 0x3C: INC_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0x05  DEC B
    case 0x05: DEC_n(Registers.B); break;
    // 0x0D  DEC C
    case 0x0D: DEC_n(Registers.C); break;
    // 0x15  DEC D
    case 0x15: DEC_n(Registers.D); break;
    // 0x1D  DEC E
    case 0x1D: DEC_n(Registers.E); break;
    // 0x25  DEC H
    case 0x25: DEC_n(Registers.H); break;
    // 0x2D  DEC L
    case 0x2D: DEC_n(Registers.L); break;
    // 0x35  DEC (HL)
    case 0x35: {
        uint8_t val = mmu->readByte(Registers.HL);
        DEC_n(val);
        mmu->writeByte(Registers.HL, val);
        break;
    }
    // 0x3D  DEC A
    case 0x3D: DEC_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0x06  LD B,n
    case 0x06: Registers.B = n8(); break;
    // 0x0E  LD C,n
    case 0x0E: Registers.C = n8(); break;
    // 0x16  LD D,n
    case 0x16: Registers.D = n8(); break;
    // 0x1E  LD E,n
    case 0x1E: Registers.E = n8(); break;
    // 0x26  LD H,n
    case 0x26: Registers.H = n8(); break;
    // 0x2E  LD L,n
    case 0x2E: Registers.L = n8(); break;
    // 0x36  LD (HL),n
    case 0x36: mmu->writeByte(Registers.HL, n8()); break;
    // 0x3E  LD A,n
    case 0x3E: Registers.A = n8(); break;

    // -------------------------------------------------------------------------
    // 0x07  RLCA
    case 0x07: RLCA(); break;
    // 0x0F  RRCA
    case 0x0F: RRCA(); break;
    // 0x17  RLA
    case 0x17: RLA(); break;
    // 0x1F  RRA
    case 0x1F: RRA(); break;

    // -------------------------------------------------------------------------
    // 0x08  LD (nn),SP
    case 0x08: LD_nn_SP(); break;

    // -------------------------------------------------------------------------
    // 0x09  ADD HL,BC
    case 0x09: ADD_HL_nn(Registers.BC); break;
    // 0x19  ADD HL,DE
    case 0x19: ADD_HL_nn(Registers.DE); break;
    // 0x29  ADD HL,HL
    case 0x29: ADD_HL_nn(Registers.HL); break;
    // 0x39  ADD HL,SP
    case 0x39: ADD_HL_nn(Registers.SP); break;

    // -------------------------------------------------------------------------
    // 0x0A  LD A,(BC)
    case 0x0A: Registers.A = mmu->readByte(Registers.BC); break;
    // 0x1A  LD A,(DE)
    case 0x1A: Registers.A = mmu->readByte(Registers.DE); break;
    // 0x2A  LD A,(HL+)  / LDI A,(HL)
    case 0x2A: LDI_A_HL(); break;
    // 0x3A  LD A,(HL-)  / LDD A,(HL)
    case 0x3A: LDD_A_HL(); break;

    // -------------------------------------------------------------------------
    // 0x0B  DEC BC
    case 0x0B: DEC_nn(Registers.BC); break;
    // 0x1B  DEC DE
    case 0x1B: DEC_nn(Registers.DE); break;
    // 0x2B  DEC HL
    case 0x2B: DEC_nn(Registers.HL); break;
    // 0x3B  DEC SP
    case 0x3B: DEC_nn(Registers.SP); break;

    // -------------------------------------------------------------------------
    // 0x10  STOP
    case 0x10: STOP(); break;

    // -------------------------------------------------------------------------
    // 0x18  JR n
    case 0x18: JR_n(); break;
    // 0x20  JR NZ,n
    case 0x20: cycleCount = JR_cc_n(0); break;
    // 0x28  JR Z,n
    case 0x28: cycleCount = JR_cc_n(1); break;
    // 0x30  JR NC,n
    case 0x30: cycleCount = JR_cc_n(2); break;
    // 0x38  JR C,n
    case 0x38: cycleCount = JR_cc_n(3); break;

    // -------------------------------------------------------------------------
    // 0x27  DAA
    case 0x27: DAA(); break;
    // 0x2F  CPL
    case 0x2F: CPL(); break;
    // 0x37  SCF
    case 0x37: SCF(); break;
    // 0x3F  CCF
    case 0x3F: CCF(); break;

    // -------------------------------------------------------------------------
    // 0x76  HALT  (sits inside the LD block 0x40-0x7F)
    case 0x76: HALT(); break;

    // -------------------------------------------------------------------------
    // 0x40-0x7F  LD r,r  (excluding 0x76 HALT above)
    // Row 0x40 ? LD B,x
    case 0x40: Registers.B = Registers.B; break;
    case 0x41: Registers.B = Registers.C; break;
    case 0x42: Registers.B = Registers.D; break;
    case 0x43: Registers.B = Registers.E; break;
    case 0x44: Registers.B = Registers.H; break;
    case 0x45: Registers.B = Registers.L; break;
    case 0x46: Registers.B = mmu->readByte(Registers.HL); break;
    case 0x47: Registers.B = Registers.A; break;
    // Row 0x48 ? LD C,x
    case 0x48: Registers.C = Registers.B; break;
    case 0x49: Registers.C = Registers.C; break;
    case 0x4A: Registers.C = Registers.D; break;
    case 0x4B: Registers.C = Registers.E; break;
    case 0x4C: Registers.C = Registers.H; break;
    case 0x4D: Registers.C = Registers.L; break;
    case 0x4E: Registers.C = mmu->readByte(Registers.HL); break;
    case 0x4F: Registers.C = Registers.A; break;
    // Row 0x50 ? LD D,x
    case 0x50: Registers.D = Registers.B; break;
    case 0x51: Registers.D = Registers.C; break;
    case 0x52: Registers.D = Registers.D; break;
    case 0x53: Registers.D = Registers.E; break;
    case 0x54: Registers.D = Registers.H; break;
    case 0x55: Registers.D = Registers.L; break;
    case 0x56: Registers.D = mmu->readByte(Registers.HL); break;
    case 0x57: Registers.D = Registers.A; break;
    // Row 0x58 ? LD E,x
    case 0x58: Registers.E = Registers.B; break;
    case 0x59: Registers.E = Registers.C; break;
    case 0x5A: Registers.E = Registers.D; break;
    case 0x5B: Registers.E = Registers.E; break;
    case 0x5C: Registers.E = Registers.H; break;
    case 0x5D: Registers.E = Registers.L; break;
    case 0x5E: Registers.E = mmu->readByte(Registers.HL); break;
    case 0x5F: Registers.E = Registers.A; break;
    // Row 0x60 ? LD H,x
    case 0x60: Registers.H = Registers.B; break;
    case 0x61: Registers.H = Registers.C; break;
    case 0x62: Registers.H = Registers.D; break;
    case 0x63: Registers.H = Registers.E; break;
    case 0x64: Registers.H = Registers.H; break;
    case 0x65: Registers.H = Registers.L; break;
    case 0x66: Registers.H = mmu->readByte(Registers.HL); break;
    case 0x67: Registers.H = Registers.A; break;
    // Row 0x68 ? LD L,x
    case 0x68: Registers.L = Registers.B; break;
    case 0x69: Registers.L = Registers.C; break;
    case 0x6A: Registers.L = Registers.D; break;
    case 0x6B: Registers.L = Registers.E; break;
    case 0x6C: Registers.L = Registers.H; break;
    case 0x6D: Registers.L = Registers.L; break;
    case 0x6E: Registers.L = mmu->readByte(Registers.HL); break;
    case 0x6F: Registers.L = Registers.A; break;
    // Row 0x70 ? LD (HL),x
    case 0x70: mmu->writeByte(Registers.HL, Registers.B); break;
    case 0x71: mmu->writeByte(Registers.HL, Registers.C); break;
    case 0x72: mmu->writeByte(Registers.HL, Registers.D); break;
    case 0x73: mmu->writeByte(Registers.HL, Registers.E); break;
    case 0x74: mmu->writeByte(Registers.HL, Registers.H); break;
    case 0x75: mmu->writeByte(Registers.HL, Registers.L); break;
    // 0x76 HALT handled above
    case 0x77: mmu->writeByte(Registers.HL, Registers.A); break;
    // Row 0x78 ? LD A,x
    case 0x78: Registers.A = Registers.B; break;
    case 0x79: Registers.A = Registers.C; break;
    case 0x7A: Registers.A = Registers.D; break;
    case 0x7B: Registers.A = Registers.E; break;
    case 0x7C: Registers.A = Registers.H; break;
    case 0x7D: Registers.A = Registers.L; break;
    case 0x7E: Registers.A = mmu->readByte(Registers.HL); break;
    case 0x7F: Registers.A = Registers.A; break;

    // -------------------------------------------------------------------------
    // 0x80-0x87  ADD A,x
    case 0x80: ADD_A_n(Registers.B); break;
    case 0x81: ADD_A_n(Registers.C); break;
    case 0x82: ADD_A_n(Registers.D); break;
    case 0x83: ADD_A_n(Registers.E); break;
    case 0x84: ADD_A_n(Registers.H); break;
    case 0x85: ADD_A_n(Registers.L); break;
    case 0x86: ADD_A_n(mmu->readByte(Registers.HL)); break;
    case 0x87: ADD_A_n(Registers.A); break;
    // 0x88-0x8F  ADC A,x
    case 0x88: ADC_A_n(Registers.B); break;
    case 0x89: ADC_A_n(Registers.C); break;
    case 0x8A: ADC_A_n(Registers.D); break;
    case 0x8B: ADC_A_n(Registers.E); break;
    case 0x8C: ADC_A_n(Registers.H); break;
    case 0x8D: ADC_A_n(Registers.L); break;
    case 0x8E: ADC_A_n(mmu->readByte(Registers.HL)); break;
    case 0x8F: ADC_A_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0x90-0x97  SUB A,x
    case 0x90: SUB_A_n(Registers.B); break;
    case 0x91: SUB_A_n(Registers.C); break;
    case 0x92: SUB_A_n(Registers.D); break;
    case 0x93: SUB_A_n(Registers.E); break;
    case 0x94: SUB_A_n(Registers.H); break;
    case 0x95: SUB_A_n(Registers.L); break;
    case 0x96: SUB_A_n(mmu->readByte(Registers.HL)); break;
    case 0x97: SUB_A_n(Registers.A); break;
    // 0x98-0x9F  SBC A,x
    case 0x98: SBC_A_n(Registers.B); break;
    case 0x99: SBC_A_n(Registers.C); break;
    case 0x9A: SBC_A_n(Registers.D); break;
    case 0x9B: SBC_A_n(Registers.E); break;
    case 0x9C: SBC_A_n(Registers.H); break;
    case 0x9D: SBC_A_n(Registers.L); break;
    case 0x9E: SBC_A_n(mmu->readByte(Registers.HL)); break;
    case 0x9F: SBC_A_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0xA0-0xA7  AND A,x
    case 0xA0: AND_A_n(Registers.B); break;
    case 0xA1: AND_A_n(Registers.C); break;
    case 0xA2: AND_A_n(Registers.D); break;
    case 0xA3: AND_A_n(Registers.E); break;
    case 0xA4: AND_A_n(Registers.H); break;
    case 0xA5: AND_A_n(Registers.L); break;
    case 0xA6: AND_A_n(mmu->readByte(Registers.HL)); break;
    case 0xA7: AND_A_n(Registers.A); break;
    // 0xA8-0xAF  XOR A,x
    case 0xA8: XOR_A_n(Registers.B); break;
    case 0xA9: XOR_A_n(Registers.C); break;
    case 0xAA: XOR_A_n(Registers.D); break;
    case 0xAB: XOR_A_n(Registers.E); break;
    case 0xAC: XOR_A_n(Registers.H); break;
    case 0xAD: XOR_A_n(Registers.L); break;
    case 0xAE: XOR_A_n(mmu->readByte(Registers.HL)); break;
    case 0xAF: XOR_A_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0xB0-0xB7  OR A,x
    case 0xB0: OR_A_n(Registers.B); break;
    case 0xB1: OR_A_n(Registers.C); break;
    case 0xB2: OR_A_n(Registers.D); break;
    case 0xB3: OR_A_n(Registers.E); break;
    case 0xB4: OR_A_n(Registers.H); break;
    case 0xB5: OR_A_n(Registers.L); break;
    case 0xB6: OR_A_n(mmu->readByte(Registers.HL)); break;
    case 0xB7: OR_A_n(Registers.A); break;
    // 0xB8-0xBF  CP A,x
    case 0xB8: CP_A_n(Registers.B); break;
    case 0xB9: CP_A_n(Registers.C); break;
    case 0xBA: CP_A_n(Registers.D); break;
    case 0xBB: CP_A_n(Registers.E); break;
    case 0xBC: CP_A_n(Registers.H); break;
    case 0xBD: CP_A_n(Registers.L); break;
    case 0xBE: CP_A_n(mmu->readByte(Registers.HL)); break;
    case 0xBF: CP_A_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0xC0  RET NZ
    case 0xC0: cycleCount = RET_cc(0); break;
    // 0xC1  POP BC
    case 0xC1: POP_nn(Registers.BC); break;
    // 0xC2  JP NZ,nn
    case 0xC2: cycleCount = JP_cc_nn(0); break;
    // 0xC3  JP nn
    case 0xC3: JP_nn(n16()); break;
    // 0xC4  CALL NZ,nn
    case 0xC4: cycleCount = CALL_cc_nn(0); break;
    // 0xC5  PUSH BC
    case 0xC5: PUSH_nn(Registers.BC); break;
    // 0xC6  ADD A,n
    case 0xC6: ADD_A_n(n8()); break;
    // 0xC7  RST 00H
    case 0xC7: RST_n(0x00); break;
    // 0xC8  RET Z
    case 0xC8: cycleCount = RET_cc(1); break;
    // 0xC9  RET
    case 0xC9: RET(); break;
    // 0xCA  JP Z,nn
    case 0xCA: cycleCount = JP_cc_nn(1); break;
    // 0xCB  PREFIX CB
    case 0xCB: executePrefixedInstruction(); break;
    // 0xCC  CALL Z,nn
    case 0xCC: cycleCount = CALL_cc_nn(1); break;
    // 0xCD  CALL nn
    case 0xCD: CALL_nn(); break;
    // 0xCE  ADC A,n
    case 0xCE: ADC_A_n(n8()); break;
    // 0xCF  RST 08H
    case 0xCF: RST_n(0x08); break;

    // -------------------------------------------------------------------------
    // 0xD0  RET NC
    case 0xD0: cycleCount = RET_cc(2); break;
    // 0xD1  POP DE
    case 0xD1: POP_nn(Registers.DE); break;
    // 0xD2  JP NC,nn
    case 0xD2: cycleCount = JP_cc_nn(2); break;
    // 0xD3  ILLEGAL
    // 0xD4  CALL NC,nn
    case 0xD4: cycleCount = CALL_cc_nn(2); break;
    // 0xD5  PUSH DE
    case 0xD5: PUSH_nn(Registers.DE); break;
    // 0xD6  SUB A,n
    case 0xD6: SUB_A_n(n8()); break;
    // 0xD7  RST 10H
    case 0xD7: RST_n(0x10); break;
    // 0xD8  RET C
    case 0xD8: cycleCount = RET_cc(3); break;
    // 0xD9  RETI
    case 0xD9: RETI(); break;
    // 0xDA  JP C,nn
    case 0xDA: cycleCount = JP_cc_nn(3); break;
    // 0xDB  ILLEGAL
    // 0xDC  CALL C,nn
    case 0xDC: cycleCount = CALL_cc_nn(3); break;
    // 0xDD  ILLEGAL
    // 0xDE  SBC A,n
    case 0xDE: SBC_A_n(n8()); break;
    // 0xDF  RST 18H
    case 0xDF: RST_n(0x18); break;

    // -------------------------------------------------------------------------
    // 0xE0  LDH (n),A
    case 0xE0: LDH_n_A(); break;
    // 0xE1  POP HL
    case 0xE1: POP_nn(Registers.HL); break;
    // 0xE2  LD (C),A
    case 0xE2: LD_C_A(); break;
    // 0xE3  ILLEGAL
    // 0xE4  ILLEGAL
    // 0xE5  PUSH HL
    case 0xE5: PUSH_nn(Registers.HL); break;
    // 0xE6  AND A,n
    case 0xE6: AND_A_n(n8()); break;
    // 0xE7  RST 20H
    case 0xE7: RST_n(0x20); break;
    // 0xE8  ADD SP,n
    case 0xE8: ADD_SP_n(); break;
    // 0xE9  JP HL
    case 0xE9: JP_HL(); break;
    // 0xEA  LD (nn),A
    case 0xEA: mmu->writeByte(n16(), Registers.A); break;
    // 0xEB  ILLEGAL
    // 0xEC  ILLEGAL
    // 0xED  ILLEGAL
    // 0xEE  XOR A,n
    case 0xEE: XOR_A_n(n8()); break;
    // 0xEF  RST 28H
    case 0xEF: RST_n(0x28); break;

    // -------------------------------------------------------------------------
    // 0xF0  LDH A,(n)
    case 0xF0: LDH_A_n(); break;
    // 0xF1  POP AF
    case 0xF1: POP_nn(Registers.AF); break;
    // 0xF2  LD A,(C)
    case 0xF2: LD_A_C(); break;
    // 0xF3  DI
    case 0xF3: DI(); break;
    // 0xF4  ILLEGAL
    // 0xF5  PUSH AF
    case 0xF5: PUSH_nn(Registers.AF); break;
    // 0xF6  OR A,n
    case 0xF6: OR_A_n(n8()); break;
    // 0xF7  RST 30H
    case 0xF7: RST_n(0x30); break;
    // 0xF8  LD HL,SP+n
    case 0xF8: LD_HL_SP_n(); break;
    // 0xF9  LD SP,HL
    case 0xF9: LD_SP_HL(); break;
    // 0xFA  LD A,(nn)
    case 0xFA: Registers.A = mmu->readByte(n16()); break;
    // 0xFB  EI
    case 0xFB: EI(); break;
    // 0xFC  ILLEGAL
    // 0xFD  ILLEGAL
    // 0xFE  CP A,n
    case 0xFE: CP_A_n(n8()); break;
    // 0xFF  RST 38H
    case 0xFF: RST_n(0x38); break;

    default:
        // Illegal/undefined opcode ? treat as NOP to avoid undefined behaviour.
        break;
    }
}

void SM83::executePrefixedInstruction() {
    uint8_t opcode = n8();
    cycleCount = CGB_PrefixedInstruction_Cycle[opcode];

    switch (opcode) {

    // -------------------------------------------------------------------------
    // 0xCB 0x00-0x07  RLC r
    case 0x00: RLC_n(Registers.B); break;
    case 0x01: RLC_n(Registers.C); break;
    case 0x02: RLC_n(Registers.D); break;
    case 0x03: RLC_n(Registers.E); break;
    case 0x04: RLC_n(Registers.H); break;
    case 0x05: RLC_n(Registers.L); break;
    case 0x06: { uint8_t v = mmu->readByte(Registers.HL); RLC_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x07: RLC_n(Registers.A); break;

    // 0xCB 0x08-0x0F  RRC r
    case 0x08: RRC_n(Registers.B); break;
    case 0x09: RRC_n(Registers.C); break;
    case 0x0A: RRC_n(Registers.D); break;
    case 0x0B: RRC_n(Registers.E); break;
    case 0x0C: RRC_n(Registers.H); break;
    case 0x0D: RRC_n(Registers.L); break;
    case 0x0E: { uint8_t v = mmu->readByte(Registers.HL); RRC_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x0F: RRC_n(Registers.A); break;

    // 0xCB 0x10-0x17  RL r
    case 0x10: RL_n(Registers.B); break;
    case 0x11: RL_n(Registers.C); break;
    case 0x12: RL_n(Registers.D); break;
    case 0x13: RL_n(Registers.E); break;
    case 0x14: RL_n(Registers.H); break;
    case 0x15: RL_n(Registers.L); break;
    case 0x16: { uint8_t v = mmu->readByte(Registers.HL); RL_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x17: RL_n(Registers.A); break;

    // 0xCB 0x18-0x1F  RR r
    case 0x18: RR_n(Registers.B); break;
    case 0x19: RR_n(Registers.C); break;
    case 0x1A: RR_n(Registers.D); break;
    case 0x1B: RR_n(Registers.E); break;
    case 0x1C: RR_n(Registers.H); break;
    case 0x1D: RR_n(Registers.L); break;
    case 0x1E: { uint8_t v = mmu->readByte(Registers.HL); RR_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x1F: RR_n(Registers.A); break;

    // 0xCB 0x20-0x27  SLA r
    case 0x20: SLA_n(Registers.B); break;
    case 0x21: SLA_n(Registers.C); break;
    case 0x22: SLA_n(Registers.D); break;
    case 0x23: SLA_n(Registers.E); break;
    case 0x24: SLA_n(Registers.H); break;
    case 0x25: SLA_n(Registers.L); break;
    case 0x26: { uint8_t v = mmu->readByte(Registers.HL); SLA_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x27: SLA_n(Registers.A); break;

    // 0xCB 0x28-0x2F  SRA r
    case 0x28: SRA_n(Registers.B); break;
    case 0x29: SRA_n(Registers.C); break;
    case 0x2A: SRA_n(Registers.D); break;
    case 0x2B: SRA_n(Registers.E); break;
    case 0x2C: SRA_n(Registers.H); break;
    case 0x2D: SRA_n(Registers.L); break;
    case 0x2E: { uint8_t v = mmu->readByte(Registers.HL); SRA_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x2F: SRA_n(Registers.A); break;

    // 0xCB 0x30-0x37  SWAP r
    case 0x30: SWAP_n(Registers.B); break;
    case 0x31: SWAP_n(Registers.C); break;
    case 0x32: SWAP_n(Registers.D); break;
    case 0x33: SWAP_n(Registers.E); break;
    case 0x34: SWAP_n(Registers.H); break;
    case 0x35: SWAP_n(Registers.L); break;
    case 0x36: { uint8_t v = mmu->readByte(Registers.HL); SWAP_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x37: SWAP_n(Registers.A); break;

    // 0xCB 0x38-0x3F  SRL r
    case 0x38: SRL_n(Registers.B); break;
    case 0x39: SRL_n(Registers.C); break;
    case 0x3A: SRL_n(Registers.D); break;
    case 0x3B: SRL_n(Registers.E); break;
    case 0x3C: SRL_n(Registers.H); break;
    case 0x3D: SRL_n(Registers.L); break;
    case 0x3E: { uint8_t v = mmu->readByte(Registers.HL); SRL_n(v); mmu->writeByte(Registers.HL, v); break; }
    case 0x3F: SRL_n(Registers.A); break;

    // -------------------------------------------------------------------------
    // 0xCB 0x40-0x47  BIT 0,r
    case 0x40: BIT_b_r(0, Registers.B); break;
    case 0x41: BIT_b_r(0, Registers.C); break;
    case 0x42: BIT_b_r(0, Registers.D); break;
    case 0x43: BIT_b_r(0, Registers.E); break;
    case 0x44: BIT_b_r(0, Registers.H); break;
    case 0x45: BIT_b_r(0, Registers.L); break;
    case 0x46: BIT_b_r(0, mmu->readByte(Registers.HL)); break;
    case 0x47: BIT_b_r(0, Registers.A); break;
    // 0xCB 0x48-0x4F  BIT 1,r
    case 0x48: BIT_b_r(1, Registers.B); break;
    case 0x49: BIT_b_r(1, Registers.C); break;
    case 0x4A: BIT_b_r(1, Registers.D); break;
    case 0x4B: BIT_b_r(1, Registers.E); break;
    case 0x4C: BIT_b_r(1, Registers.H); break;
    case 0x4D: BIT_b_r(1, Registers.L); break;
    case 0x4E: BIT_b_r(1, mmu->readByte(Registers.HL)); break;
    case 0x4F: BIT_b_r(1, Registers.A); break;
    // 0xCB 0x50-0x57  BIT 2,r
    case 0x50: BIT_b_r(2, Registers.B); break;
    case 0x51: BIT_b_r(2, Registers.C); break;
    case 0x52: BIT_b_r(2, Registers.D); break;
    case 0x53: BIT_b_r(2, Registers.E); break;
    case 0x54: BIT_b_r(2, Registers.H); break;
    case 0x55: BIT_b_r(2, Registers.L); break;
    case 0x56: BIT_b_r(2, mmu->readByte(Registers.HL)); break;
    case 0x57: BIT_b_r(2, Registers.A); break;
    // 0xCB 0x58-0x5F  BIT 3,r
    case 0x58: BIT_b_r(3, Registers.B); break;
    case 0x59: BIT_b_r(3, Registers.C); break;
    case 0x5A: BIT_b_r(3, Registers.D); break;
    case 0x5B: BIT_b_r(3, Registers.E); break;
    case 0x5C: BIT_b_r(3, Registers.H); break;
    case 0x5D: BIT_b_r(3, Registers.L); break;
    case 0x5E: BIT_b_r(3, mmu->readByte(Registers.HL)); break;
    case 0x5F: BIT_b_r(3, Registers.A); break;
    // 0xCB 0x60-0x67  BIT 4,r
    case 0x60: BIT_b_r(4, Registers.B); break;
    case 0x61: BIT_b_r(4, Registers.C); break;
    case 0x62: BIT_b_r(4, Registers.D); break;
    case 0x63: BIT_b_r(4, Registers.E); break;
    case 0x64: BIT_b_r(4, Registers.H); break;
    case 0x65: BIT_b_r(4, Registers.L); break;
    case 0x66: BIT_b_r(4, mmu->readByte(Registers.HL)); break;
    case 0x67: BIT_b_r(4, Registers.A); break;
    // 0xCB 0x68-0x6F  BIT 5,r
    case 0x68: BIT_b_r(5, Registers.B); break;
    case 0x69: BIT_b_r(5, Registers.C); break;
    case 0x6A: BIT_b_r(5, Registers.D); break;
    case 0x6B: BIT_b_r(5, Registers.E); break;
    case 0x6C: BIT_b_r(5, Registers.H); break;
    case 0x6D: BIT_b_r(5, Registers.L); break;
    case 0x6E: BIT_b_r(5, mmu->readByte(Registers.HL)); break;
    case 0x6F: BIT_b_r(5, Registers.A); break;
    // 0xCB 0x70-0x77  BIT 6,r
    case 0x70: BIT_b_r(6, Registers.B); break;
    case 0x71: BIT_b_r(6, Registers.C); break;
    case 0x72: BIT_b_r(6, Registers.D); break;
    case 0x73: BIT_b_r(6, Registers.E); break;
    case 0x74: BIT_b_r(6, Registers.H); break;
    case 0x75: BIT_b_r(6, Registers.L); break;
    case 0x76: BIT_b_r(6, mmu->readByte(Registers.HL)); break;
    case 0x77: BIT_b_r(6, Registers.A); break;
    // 0xCB 0x78-0x7F  BIT 7,r
    case 0x78: BIT_b_r(7, Registers.B); break;
    case 0x79: BIT_b_r(7, Registers.C); break;
    case 0x7A: BIT_b_r(7, Registers.D); break;
    case 0x7B: BIT_b_r(7, Registers.E); break;
    case 0x7C: BIT_b_r(7, Registers.H); break;
    case 0x7D: BIT_b_r(7, Registers.L); break;
    case 0x7E: BIT_b_r(7, mmu->readByte(Registers.HL)); break;
    case 0x7F: BIT_b_r(7, Registers.A); break;

    // -------------------------------------------------------------------------
    // 0xCB 0x80-0x87  RES 0,r
    case 0x80: RES_b_r(0, Registers.B); break;
    case 0x81: RES_b_r(0, Registers.C); break;
    case 0x82: RES_b_r(0, Registers.D); break;
    case 0x83: RES_b_r(0, Registers.E); break;
    case 0x84: RES_b_r(0, Registers.H); break;
    case 0x85: RES_b_r(0, Registers.L); break;
    case 0x86: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(0, v); mmu->writeByte(Registers.HL, v); break; }
    case 0x87: RES_b_r(0, Registers.A); break;
    // 0xCB 0x88-0x8F  RES 1,r
    case 0x88: RES_b_r(1, Registers.B); break;
    case 0x89: RES_b_r(1, Registers.C); break;
    case 0x8A: RES_b_r(1, Registers.D); break;
    case 0x8B: RES_b_r(1, Registers.E); break;
    case 0x8C: RES_b_r(1, Registers.H); break;
    case 0x8D: RES_b_r(1, Registers.L); break;
    case 0x8E: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(1, v); mmu->writeByte(Registers.HL, v); break; }
    case 0x8F: RES_b_r(1, Registers.A); break;
    // 0xCB 0x90-0x97  RES 2,r
    case 0x90: RES_b_r(2, Registers.B); break;
    case 0x91: RES_b_r(2, Registers.C); break;
    case 0x92: RES_b_r(2, Registers.D); break;
    case 0x93: RES_b_r(2, Registers.E); break;
    case 0x94: RES_b_r(2, Registers.H); break;
    case 0x95: RES_b_r(2, Registers.L); break;
    case 0x96: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(2, v); mmu->writeByte(Registers.HL, v); break; }
    case 0x97: RES_b_r(2, Registers.A); break;
    // 0xCB 0x98-0x9F  RES 3,r
    case 0x98: RES_b_r(3, Registers.B); break;
    case 0x99: RES_b_r(3, Registers.C); break;
    case 0x9A: RES_b_r(3, Registers.D); break;
    case 0x9B: RES_b_r(3, Registers.E); break;
    case 0x9C: RES_b_r(3, Registers.H); break;
    case 0x9D: RES_b_r(3, Registers.L); break;
    case 0x9E: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(3, v); mmu->writeByte(Registers.HL, v); break; }
    case 0x9F: RES_b_r(3, Registers.A); break;
    // 0xCB 0xA0-0xA7  RES 4,r
    case 0xA0: RES_b_r(4, Registers.B); break;
    case 0xA1: RES_b_r(4, Registers.C); break;
    case 0xA2: RES_b_r(4, Registers.D); break;
    case 0xA3: RES_b_r(4, Registers.E); break;
    case 0xA4: RES_b_r(4, Registers.H); break;
    case 0xA5: RES_b_r(4, Registers.L); break;
    case 0xA6: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(4, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xA7: RES_b_r(4, Registers.A); break;
    // 0xCB 0xA8-0xAF  RES 5,r
    case 0xA8: RES_b_r(5, Registers.B); break;
    case 0xA9: RES_b_r(5, Registers.C); break;
    case 0xAA: RES_b_r(5, Registers.D); break;
    case 0xAB: RES_b_r(5, Registers.E); break;
    case 0xAC: RES_b_r(5, Registers.H); break;
    case 0xAD: RES_b_r(5, Registers.L); break;
    case 0xAE: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(5, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xAF: RES_b_r(5, Registers.A); break;
    // 0xCB 0xB0-0xB7  RES 6,r
    case 0xB0: RES_b_r(6, Registers.B); break;
    case 0xB1: RES_b_r(6, Registers.C); break;
    case 0xB2: RES_b_r(6, Registers.D); break;
    case 0xB3: RES_b_r(6, Registers.E); break;
    case 0xB4: RES_b_r(6, Registers.H); break;
    case 0xB5: RES_b_r(6, Registers.L); break;
    case 0xB6: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(6, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xB7: RES_b_r(6, Registers.A); break;
    // 0xCB 0xB8-0xBF  RES 7,r
    case 0xB8: RES_b_r(7, Registers.B); break;
    case 0xB9: RES_b_r(7, Registers.C); break;
    case 0xBA: RES_b_r(7, Registers.D); break;
    case 0xBB: RES_b_r(7, Registers.E); break;
    case 0xBC: RES_b_r(7, Registers.H); break;
    case 0xBD: RES_b_r(7, Registers.L); break;
    case 0xBE: { uint8_t v = mmu->readByte(Registers.HL); RES_b_r(7, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xBF: RES_b_r(7, Registers.A); break;

    // -------------------------------------------------------------------------
    // 0xCB 0xC0-0xC7  SET 0,r
    case 0xC0: SET_b_r(0, Registers.B); break;
    case 0xC1: SET_b_r(0, Registers.C); break;
    case 0xC2: SET_b_r(0, Registers.D); break;
    case 0xC3: SET_b_r(0, Registers.E); break;
    case 0xC4: SET_b_r(0, Registers.H); break;
    case 0xC5: SET_b_r(0, Registers.L); break;
    case 0xC6: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(0, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xC7: SET_b_r(0, Registers.A); break;
    // 0xCB 0xC8-0xCF  SET 1,r
    case 0xC8: SET_b_r(1, Registers.B); break;
    case 0xC9: SET_b_r(1, Registers.C); break;
    case 0xCA: SET_b_r(1, Registers.D); break;
    case 0xCB: SET_b_r(1, Registers.E); break;
    case 0xCC: SET_b_r(1, Registers.H); break;
    case 0xCD: SET_b_r(1, Registers.L); break;
    case 0xCE: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(1, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xCF: SET_b_r(1, Registers.A); break;
    // 0xCB 0xD0-0xD7  SET 2,r
    case 0xD0: SET_b_r(2, Registers.B); break;
    case 0xD1: SET_b_r(2, Registers.C); break;
    case 0xD2: SET_b_r(2, Registers.D); break;
    case 0xD3: SET_b_r(2, Registers.E); break;
    case 0xD4: SET_b_r(2, Registers.H); break;
    case 0xD5: SET_b_r(2, Registers.L); break;
    case 0xD6: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(2, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xD7: SET_b_r(2, Registers.A); break;
    // 0xCB 0xD8-0xDF  SET 3,r
    case 0xD8: SET_b_r(3, Registers.B); break;
    case 0xD9: SET_b_r(3, Registers.C); break;
    case 0xDA: SET_b_r(3, Registers.D); break;
    case 0xDB: SET_b_r(3, Registers.E); break;
    case 0xDC: SET_b_r(3, Registers.H); break;
    case 0xDD: SET_b_r(3, Registers.L); break;
    case 0xDE: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(3, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xDF: SET_b_r(3, Registers.A); break;
    // 0xCB 0xE0-0xE7  SET 4,r
    case 0xE0: SET_b_r(4, Registers.B); break;
    case 0xE1: SET_b_r(4, Registers.C); break;
    case 0xE2: SET_b_r(4, Registers.D); break;
    case 0xE3: SET_b_r(4, Registers.E); break;
    case 0xE4: SET_b_r(4, Registers.H); break;
    case 0xE5: SET_b_r(4, Registers.L); break;
    case 0xE6: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(4, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xE7: SET_b_r(4, Registers.A); break;
    // 0xCB 0xE8-0xEF  SET 5,r
    case 0xE8: SET_b_r(5, Registers.B); break;
    case 0xE9: SET_b_r(5, Registers.C); break;
    case 0xEA: SET_b_r(5, Registers.D); break;
    case 0xEB: SET_b_r(5, Registers.E); break;
    case 0xEC: SET_b_r(5, Registers.H); break;
    case 0xED: SET_b_r(5, Registers.L); break;
    case 0xEE: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(5, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xEF: SET_b_r(5, Registers.A); break;
    // 0xCB 0xF0-0xF7  SET 6,r
    case 0xF0: SET_b_r(6, Registers.B); break;
    case 0xF1: SET_b_r(6, Registers.C); break;
    case 0xF2: SET_b_r(6, Registers.D); break;
    case 0xF3: SET_b_r(6, Registers.E); break;
    case 0xF4: SET_b_r(6, Registers.H); break;
    case 0xF5: SET_b_r(6, Registers.L); break;
    case 0xF6: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(6, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xF7: SET_b_r(6, Registers.A); break;
    // 0xCB 0xF8-0xFF  SET 7,r
    case 0xF8: SET_b_r(7, Registers.B); break;
    case 0xF9: SET_b_r(7, Registers.C); break;
    case 0xFA: SET_b_r(7, Registers.D); break;
    case 0xFB: SET_b_r(7, Registers.E); break;
    case 0xFC: SET_b_r(7, Registers.H); break;
    case 0xFD: SET_b_r(7, Registers.L); break;
    case 0xFE: { uint8_t v = mmu->readByte(Registers.HL); SET_b_r(7, v); mmu->writeByte(Registers.HL, v); break; }
    case 0xFF: SET_b_r(7, Registers.A); break;

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
        // case 6 is (HL) and must be handled by the caller via readByte/writeByte.
        // Returning a dummy ref prevents undefined behaviour if called incorrectly.
        static uint8_t dummy = 0;
        return dummy;
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
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);
    Registers.setFlag(Registers.SUBTRACT_FLAG);

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

void SM83::SBC_A_n(uint8_t n) {
    uint8_t carry = Registers.checkFlag(Registers.CARRY_FLAG) ? 1 : 0;
    uint8_t tmpA = Registers.A;
    uint16_t result = tmpA - n - carry;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);
    Registers.setFlag(Registers.SUBTRACT_FLAG);

    if ((result & 0xFF) == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if ((tmpA & 0x0F) < ((n & 0x0F) + carry)) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if ((uint16_t)tmpA < ((uint16_t)n + carry)) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = static_cast<uint8_t>(result & 0xFF);
}

void SM83::AND_A_n(uint8_t n) {
    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.setFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    Registers.A &= n;

    if (Registers.A == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::OR_A_n(uint8_t n) {
    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    Registers.A |= n;

    if (Registers.A == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::XOR_A_n(uint8_t n) {
    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    Registers.A ^= n;

    if (Registers.A == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::CP_A_n(uint8_t n) {
    uint8_t tmpA = Registers.A;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);
    Registers.setFlag(Registers.SUBTRACT_FLAG);

    if (tmpA == n) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if ((tmpA & 0x0F) < (n & 0x0F)) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (tmpA < n) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }
}

void SM83::INC_n(uint8_t &n) {
    uint8_t result = n + 1;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);

    if (result == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if ((n & 0x0F) == 0x0F) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    n = result;
}

void SM83::DEC_n(uint8_t &n) {
    uint8_t result = n - 1;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.setFlag(Registers.SUBTRACT_FLAG);

    if (result == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    if ((n & 0x0F) == 0x00) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    n = result;
}
#pragma endregion

#pragma region 16-Bit ALU Instructions
void SM83::ADD_HL_nn(uint16_t nn) {
    uint32_t result = Registers.HL + nn;

    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (((Registers.HL & 0x0FFF) + (nn & 0x0FFF)) > 0x0FFF) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (result > 0xFFFF) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.HL = static_cast<uint16_t>(result & 0xFFFF);
}

void SM83::ADD_SP_n() {
    int8_t immediate = static_cast<int8_t>(n8());
    uint16_t tmpSP = Registers.SP;
    uint16_t result = tmpSP + immediate;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    // Half-carry and carry are computed on the low byte only (unsigned arithmetic).
    if (((tmpSP ^ immediate ^ result) & 0x10) != 0) {
        Registers.setFlag(Registers.HALF_CARRY_FLAG);
    }

    if (((tmpSP ^ immediate ^ result) & 0x100) != 0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.SP = result;
}

void SM83::INC_nn(uint16_t &nn) {
    nn++;
}

void SM83::DEC_nn(uint16_t &nn) {
    nn--;
}
#pragma endregion

#pragma region Miscellaneous Instructions
void SM83::SWAP_n(uint8_t &n) {
    uint8_t result = ((n & 0x0F) << 4) | ((n & 0xF0) >> 4);

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (result == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }

    n = result;
}

void SM83::DAA() {
    uint8_t correction = 0;
    bool carry = Registers.checkFlag(Registers.CARRY_FLAG);

    if (!Registers.checkFlag(Registers.SUBTRACT_FLAG)) {
        if (Registers.checkFlag(Registers.HALF_CARRY_FLAG) || (Registers.A & 0x0F) > 9) {
            correction |= 0x06;
        }

        if (carry || (Registers.A > 0x99)) {
            correction |= 0x60;
            carry = true;
        }

        Registers.A += correction;
    } else {
        if (Registers.checkFlag(Registers.HALF_CARRY_FLAG)) {
            correction |= 0x06;
        }
        if (carry) {
            correction |= 0x60;
        }
        Registers.A -= correction;
    }

    Registers.clearFlag(Registers.ZERO_FLAG);
    if (Registers.A == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);

    if (carry) {
        Registers.setFlag(Registers.CARRY_FLAG);
    } else {
        Registers.clearFlag(Registers.CARRY_FLAG);
    }
}

void SM83::CPL() {
    Registers.A = ~Registers.A;

    Registers.setFlag(Registers.SUBTRACT_FLAG);
    Registers.setFlag(Registers.HALF_CARRY_FLAG);
}

void SM83::CCF() {
    bool carry = Registers.checkFlag(Registers.CARRY_FLAG);

    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);

    if (carry) {
        Registers.clearFlag(Registers.CARRY_FLAG);
    } else {
        Registers.setFlag(Registers.CARRY_FLAG);
    }
}

void SM83::SCF() {
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.setFlag(Registers.CARRY_FLAG);
}

void SM83::NOP() {}

void SM83::HALT() {
    Registers.isHalted = true;
    bool interruptEnabled = Registers.IME;
    bool interruptPending = (mmu->readByte(IFaddress) & mmu->readByte(IEaddress)) != 0;

    if (!interruptEnabled && interruptPending) {
        Registers.isHalted = false;
    }
}

void SM83::STOP() {
    uint8_t key1 = mmu->readByte(KEY1address);

    if ((key1 & 0x1) == 0x1) {
        Registers.DoubleSpeed = !Registers.DoubleSpeed;
    }

    mmu->writeByte(KEY1address, (key1 & 0x7E) | ((Registers.DoubleSpeed ? 1 : 0) << 7));
}

void SM83::DI() {
    Registers.IME = false;
}

void SM83::EI() {
    // Interrupts are enabled after the *next* instruction, not immediately.
    // The actual IME = true is applied in step() after executeInstruction().
    Registers.IME_pending = true;
}
#pragma endregion

#pragma region Rotates & Shifts
void SM83::RLCA() {
    uint8_t bit7 = (Registers.A >> 7) & 0x01;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit7) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = (Registers.A << 1) | bit7;
}

void SM83::RLA() {
    uint8_t bit7   = (Registers.A >> 7) & 0x01;
    uint8_t oldCry = Registers.checkFlag(Registers.CARRY_FLAG) ? 1 : 0;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit7) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = (Registers.A << 1) | oldCry;
}

void SM83::RRCA() {
    uint8_t bit0 = Registers.A & 0x01;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = (bit0 << 7) | (Registers.A >> 1);
}

void SM83::RRA() {
    uint8_t bit0   = Registers.A & 0x01;
    uint8_t oldCry = Registers.checkFlag(Registers.CARRY_FLAG) ? 1 : 0;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    Registers.A = (oldCry << 7) | (Registers.A >> 1);
}

// CB-prefixed rotates ? these DO set ZERO_FLAG unlike the A-register variants above.
void SM83::RLC_n(uint8_t &n) {
    uint8_t bit7 = (n >> 7) & 0x01;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit7) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = (n << 1) | bit7;

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::RL_n(uint8_t &n) {
    uint8_t bit7   = (n >> 7) & 0x01;
    uint8_t oldCry = Registers.checkFlag(Registers.CARRY_FLAG) ? 1 : 0;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit7) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = (n << 1) | oldCry;

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::RRC_n(uint8_t &n) {
    uint8_t bit0 = n & 0x01;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = (bit0 << 7) | (n >> 1);

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::RR_n(uint8_t &n) {
    uint8_t bit0   = n & 0x01;
    uint8_t oldCry = Registers.checkFlag(Registers.CARRY_FLAG) ? 1 : 0;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = (oldCry << 7) | (n >> 1);

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::SLA_n(uint8_t &n) {
    uint8_t bit7 = (n >> 7) & 0x01;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit7) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = n << 1; // bit 0 is always set to 0

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::SRA_n(uint8_t &n) {
    // Arithmetic right shift: bit 7 (sign bit) is preserved.
    uint8_t bit0 = n & 0x01;
    uint8_t bit7 = n & 0x80;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = bit7 | (n >> 1);

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::SRL_n(uint8_t &n) {
    // Logical right shift: bit 7 is always set to 0.
    uint8_t bit0 = n & 0x01;

    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.clearFlag(Registers.HALF_CARRY_FLAG);
    Registers.clearFlag(Registers.CARRY_FLAG);

    if (bit0) {
        Registers.setFlag(Registers.CARRY_FLAG);
    }

    n = n >> 1;

    if (n == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}
#pragma endregion

#pragma region Bit Opcodes
void SM83::BIT_b_r(int position, uint8_t r) {
    // Tests bit at 'position' in r. Z is set if the bit is 0.
    Registers.clearFlag(Registers.ZERO_FLAG);
    Registers.clearFlag(Registers.SUBTRACT_FLAG);
    Registers.setFlag(Registers.HALF_CARRY_FLAG);
    // CARRY_FLAG is unaffected by BIT.

    if ((r & (1 << position)) == 0) {
        Registers.setFlag(Registers.ZERO_FLAG);
    }
}

void SM83::SET_b_r(int position, uint8_t &r) {
    r |= (1 << position);
    // SET does not affect any flags.
}

void SM83::RES_b_r(int position, uint8_t &r) {
    r &= ~(1 << position);
    // RES does not affect any flags.
}
#pragma endregion

#pragma region Jumps
void SM83::JP_nn(uint16_t nn) {
    Registers.PC = nn;
}

int SM83::JP_cc_nn(int cc) {
    uint16_t addr = n16();
    bool taken = false;

    switch (cc) {
    case 0: taken = !Registers.checkFlag(Registers.ZERO_FLAG);  break; // NZ
    case 1: taken =  Registers.checkFlag(Registers.ZERO_FLAG);  break; // Z
    case 2: taken = !Registers.checkFlag(Registers.CARRY_FLAG); break; // NC
    case 3: taken =  Registers.checkFlag(Registers.CARRY_FLAG); break; // C
    }

    if (taken) {
        Registers.PC = addr;
        return 16; // taken: 16 cycles
    }
    return 12; // not-taken: 12 cycles
}

void SM83::JP_HL() {
    Registers.PC = Registers.HL;
}

void SM83::JR_n() {
    int8_t offset = static_cast<int8_t>(n8());
    Registers.PC += offset;
}

int SM83::JR_cc_n(int cc) {
    int8_t offset = static_cast<int8_t>(n8());
    bool taken = false;

    switch (cc) {
    case 0: taken = !Registers.checkFlag(Registers.ZERO_FLAG);  break; // NZ
    case 1: taken =  Registers.checkFlag(Registers.ZERO_FLAG);  break; // Z
    case 2: taken = !Registers.checkFlag(Registers.CARRY_FLAG); break; // NC
    case 3: taken =  Registers.checkFlag(Registers.CARRY_FLAG); break; // C
    }

    if (taken) {
        Registers.PC += offset;
        return 12; // taken: 12 cycles
    }
    return 8; // not-taken: 8 cycles
}
#pragma endregion

#pragma region Calls
void SM83::CALL_nn() {
    uint16_t addr = n16();
    PUSH_nn(Registers.PC);
    Registers.PC = addr;
}

int SM83::CALL_cc_nn(int cc) {
    uint16_t addr = n16();
    bool taken = false;

    switch (cc) {
    case 0: taken = !Registers.checkFlag(Registers.ZERO_FLAG);  break; // NZ
    case 1: taken =  Registers.checkFlag(Registers.ZERO_FLAG);  break; // Z
    case 2: taken = !Registers.checkFlag(Registers.CARRY_FLAG); break; // NC
    case 3: taken =  Registers.checkFlag(Registers.CARRY_FLAG); break; // C
    }

    if (taken) {
        PUSH_nn(Registers.PC);
        Registers.PC = addr;
        return 24; // taken: 24 cycles
    }
    return 12; // not-taken: 12 cycles
}
#pragma endregion

#pragma region Restarts
void SM83::RST_n(uint8_t n) {
    PUSH_nn(Registers.PC);
    Registers.PC = n; // n is the reset vector: 0x00,0x08,0x10,0x18,0x20,0x28,0x30,0x38
}
#pragma endregion

#pragma region Returns
void SM83::RET() {
    POP_nn(Registers.PC);
}

int SM83::RET_cc(int cc) {
    bool taken = false;

    switch (cc) {
    case 0: taken = !Registers.checkFlag(Registers.ZERO_FLAG);  break; // NZ
    case 1: taken =  Registers.checkFlag(Registers.ZERO_FLAG);  break; // Z
    case 2: taken = !Registers.checkFlag(Registers.CARRY_FLAG); break; // NC
    case 3: taken =  Registers.checkFlag(Registers.CARRY_FLAG); break; // C
    }

    if (taken) {
        POP_nn(Registers.PC);
        return 20; // taken: 20 cycles
    }
    return 8; // not-taken: 8 cycles
}

void SM83::RETI() {
    POP_nn(Registers.PC);
    Registers.IME = true; // Re-enable interrupts immediately on return
}
#pragma endregion