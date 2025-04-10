/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Thu Apr 10 2025>  <KGkotzamanidis>
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
#include "Interrupts.h"

Interrupts::Interrupts() {
    Registers.IF = 0x00;
    Registers.IE = 0x00;
    
    std::printf("-=BIOS class initialized=-\n");
}