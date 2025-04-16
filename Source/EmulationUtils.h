/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Wed Apr 16 2025>  <KGkotzamanidis>
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
#ifndef _EMULATION_UTILS_H_
#define _EMULATION_UTILS_H_
/* GameBoy Registers Addresses */
#define P1_JOYaddress		0xFF00  // Joypad
#define SBaddress			0xFF01  // Serial transfer data
#define SCaddress			0xFF02  // Serial transfer control
#define DIVaddress			0xFF04  // Divider register
#define TIMAaddress			0xFF05  // Timer counter
#define TMAaddress			0xFF06  // Timer modulo
#define TACaddress			0xFF07  // Timer control
#define IFaddress			0xFF0F  // Interrupt flag
#define NR10address			0xFF10  // Sound channel 1 sweep
#define NR11address			0xFF11  // Sound channel 1 length timer & duty cycle
#define NR12address			0xFF12  // Sound channel 1 volume & envelope
#define NR13address			0xFF13  // Sound channel 1 period low
#define NR14address			0xFF14  // Sound channel 1 period high & control
#define NR21address			0xFF16  // Sound channel 2 length timer & duty cycle
#define NR22address			0xFF17  // Sound channel 2 volume & envelope
#define NR23address			0xFF18  // Sound channel 2 period low
#define NR24address			0xFF19  // Sound channel 2 period high & control
#define NR30address			0xFF1A  // Sound channel 3 DAC enable
#define NR31address			0xFF1B  // Sound channel 3 length timer
#define NR32address			0xFF1C  // Sound channel 3 output level
#define NR33address			0xFF1D  // Sound channel 3 period low
#define NR34address			0xFF1E  // Sound channel 3 period high & control
#define NR41address			0xFF20  // Sound channel 4 length timer
#define NR42address			0xFF21  // Sound channel 4 volume & envelope
#define NR43address			0xFF22  // Sound channel 4 frequency & randomness
#define NR44address			0xFF23  // Sound channel 4 control
#define NR50address			0xFF24  // Master volume & VIN panning
#define NR51address			0xFF25  // Sound panning
#define NR52address			0xFF26  // Sound on/off
#define WAVE_RAMaddress		0xFF30  // Waveform storage
#define LCDCaddress			0xFF40  // LCD control
#define STATaddress			0xFF41  // LCD status
#define SCYaddress			0xFF42  // Viewport Y position
#define SCXaddress			0xFF43  // Viewport X position
#define LYaddress			0xFF44  // LCD Y coordinate
#define LYCaddress			0xFF45  // LY compare
#define DMAaddress			0xFF46  // OAM DMA source address & start
#define BGPaddress			0xFF47  // BG palette data (DMG)
#define OBP0address			0xFF48  // OBJ palette 0 data (DMG)
#define OBP1address			0xFF49  // OBJ palette 1 data (DMG)
#define WYaddress			0xFF4A  // Window Y position
#define WXaddress			0xFF4B  // Window X position plus 7
#define KEY1address			0xFF4D  // Prepare speed switch (CGB)
#define VBKaddress			0xFF4F  // VRAM bank (CGB)
#define HDMA1address		0xFF51  // VRAM DMA source high (CGB)
#define HDMA2address		0xFF52  // VRAM DMA source low (CGB)
#define HDMA3address		0xFF53  // VRAM DMA destination high (CGB)
#define HDMA4address		0xFF54  // VRAM DMA destination low (CGB)
#define HDMA5address		0xFF55  // VRAM DMA length/mode/start (CGB)
#define RPaddress			0xFF56  // Infrared communication port (CGB)
#define BGPIaddress			0xFF68  // BG color palette index (CGB)
#define BGPDaddress			0xFF69  // BG color palette data (CGB)
#define OBPIaddress			0xFF6A  // OBJ color palette index (CGB)
#define OBPDaddress			0xFF6B  // OBJ color palette data (CGB)
#define SVBKaddress			0xFF70  // WRAM bank (CGB)
#define IEaddress			0xFFFF  // Interrupt enable

/* Overflow Bytes */
#define timerOverflow		0x4
#define buttonOverflow		0x10

/* Interrupts Bytes */
#define LCDCInterrupt		0x2

#define VBlankInterrupt		0x01
#define HBlankInterrupt     0x08

/* PPU Mode */
#define mode0				0x8
#define mode1				0x10
#define mode2				0x20
#define coincidence			0x40

/* CPU */
#define Flag_Z				0x80
#define Flag_N				0x40
#define Flag_H				0x20
#define Flag_C				0x10

#define Flag_Z_reset		0x7F
#define Flag_N_reset		0xBF
#define Flag_H_reset		0xDF
#define Flag_C_reset		0xE0
#endif