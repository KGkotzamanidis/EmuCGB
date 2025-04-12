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
#ifndef _SM83_H_
#define _SM83_H_

#include "HWRegisters.h"
#include "MMU.h"

class SM83 {
public:
    SM83(MMU &mmu);

    void run(void);
    int step();

private:
    MMU *mmu = nullptr;
    CPURegisters Registers;

    int cycleCount;

    uint8_t n8();
    uint16_t n16();

    uint16_t a8();

    void executeInstruction(void);
    void executePrefixedInstruction(void);

    /*
     * Private functions
     * @note These functions are used to access the registers.
     * @param reg The register to access.
     * @return The value of the register.
     */
private:
    uint8_t &getRegisters(uint8_t reg);
    uint8_t getRegisterValue(uint8_t reg);

    /*
     * 8-Bit Loads
     */
private:
    /*
     * 8-Bit Loads
     * LD nn,n
     * @param uint8_t opcode The opcode to execute.
     * @note Put value nn into n.
     */
    void LD_nn_n(uint8_t opcode);
    /*
     * 8-Bit Loads
     * LD r1,r2
     * @param uint8_t opcode The opcode to execute.
     * @note Put value r2 into r1.
     */
    void LD_r1_r2(uint8_t opcode);
    void LD_r1_r2(uint8_t &dst, uint8_t &src);
    /*
     * 8-Bit Loads
     * LD A,n
     * @param uint8_t opcode The opcode to execute.
     * @note Put value n into A.
     */
    void LD_A_n(uint8_t opcode);
    /*
     * 8-Bit Loads
     * LD n,A
     * @param uint8_t opcode The opcode to execute.
     * @note Put value A into n.
     */
    void LD_n_A(uint8_t opcode);
    /*
     * 8-Bit Loads
     * LD_A_C
     * @param uint8_t opcode The opcode to execute.
     * @note Put value address 0xFF00 + Register C into A.
     */
    void LD_A_C(void);
    /*
     * 8-Bit Loads
     * LD_C_A
     * @note Put value A into address 0xFF00 + Register C.
     */
    void LD_C_A(void);
    /*
     * 8-Bit Loads
     * LDD_A_HL
     * @note Put value at address HL into A, Decrement HL.
     */
    void LDD_A_HL(void);
    /*
     * 8-Bit Loads
     * LDD_HL_A
     * @note Put value A into memory address HL, Decrement HL.
     */
    void LDD_HL_A(void);
    /*
     * 8-Bit Loads
     * LDI_A_HL
     * @note Put value at address HL into A, Increment HL.
     */
    void LDI_A_HL(void);
    /*
     * 8-Bit Loads
     * LDI_HL_A
     * @note Put value A into memory address HL, Increment HL.
     */
    void LDI_HL_A(void);
    /*
     * 8-Bit Loads
     * LDH_A_n
     * @note Put A into memory address 0xFF00 + n.
     */
    void LDH_n_A(void);
    /*
     * 8-Bit Loads
     * LDH_A_n
     * @note Put value at address 0xFF00 + n into A.
     */
    void LDH_A_n(void);

    /*
     * 16-Bit Loads
     */
private:
    /*
     * 16-Bit Loads
     * LD nn,nn
     * @param uint16_t &dst The destination address.
     * @note Put value nn into address nn.
     */
    void LD_n_nn(uint16_t &dst);
    /*
     * 16-Bit Loads
     * LD SP,HL
     * @note Put value HL into SP.
     */
    void LD_SP_HL(void);
    /*
     * 16-Bit Loads
     * LD HL,SP+n
     * @note Put value SP+n into HL.
     */
    void LD_HL_SP_n(void);
    /*
     * 16-Bit Loads
     * LD nn,SP
     * @note Put value SP into address nn.
     */
    void LD_nn_SP(void);
    /*
     * 16-Bit Loads
     * PUSH nn
     * @param uint16_t src The source address.
     * @note Push value nn into stack.
     */
    void PUSH_nn(uint16_t src);
    /*
     * 16-Bit Loads
     * POP nn
     * @param uint16_t &dst The destination address.
     * @note Pop value from stack into nn.
     */
    void POP_nn(uint16_t &dst);

    /*
     * 8-Bit ALU
     */
private:
    /*
    * 8-Bit ALU
    * ADD A,n
    * @param uint8_t n for n= A,B,C,D,E,H,L,(HL),#
    * @note Add n to Register A.
    */
    void ADD_A_n(uint8_t n);
    void ADC_A_n(uint8_t n);
    void SUB_A_n(uint8_t n);
    void SBC_A_n(uint8_t n);
    void AND_A_n(uint8_t n);
    void OR_A_n(uint8_t n);
    void XOR_A_n(uint8_t n);
    void CP_A_n(uint8_t n);
    void INC_n(uint8_t &n);
    void DEC_n(uint8_t &n);
};
#endif