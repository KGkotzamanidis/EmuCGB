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
#ifndef _JOYPAD_H_
#define _JOYPAD_H_

#include <SDL3/SDL.h>

#include <cstdint>
#include <iostream>

class Joypad {
public:
    Joypad();

    void writeIO(uint8_t data);
    uint8_t readIO();

    bool checkInterrupt();
    void handlerEvent(const SDL_Event &event);

private:
    uint8_t P1_JOYP = 0x30;
    uint8_t DirectionKeys = 0x0F;
    uint8_t ButtonKeys = 0x0F;

    bool RequestInterrupt = false;

    void updateState(int bit, bool isDirection,bool Pressed);
};
#endif