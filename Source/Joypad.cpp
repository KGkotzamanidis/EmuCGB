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
#include "Joypad.hpp"

Joypad::Joypad() {
    std::printf("-=Joypad class initialized=-\n");
}

void Joypad::writeIO(uint8_t data) {
    P1_JOYP = (data & 0x30);
}

uint8_t Joypad::readIO() {
    uint8_t result = 0xCF | P1_JOYP;

    if (!(P1_JOYP & 0x10)) {
        result &= DirectionKeys;
    }

    if (!(P1_JOYP & 0x20)) {
        result &= ButtonKeys;
    }

    return result;
}

bool Joypad::checkInterrupt() {
    if (RequestInterrupt) {
        RequestInterrupt = false;
        return true;
    }
    return false;
}

void Joypad::handlerEvent(const SDL_Event &event) {
    if (event.type == SDL_EVENT_KEY_DOWN || event.type == SDL_EVENT_KEY_UP) {
        bool pressed = (event.type == SDL_EVENT_KEY_DOWN);

        switch (event.key.scancode) {
        case SDL_SCANCODE_RIGHT:
            updateState(0, true, pressed);
            break;
        case SDL_SCANCODE_LEFT:
            updateState(1, true, pressed);
            break;
        case SDL_SCANCODE_UP:
            updateState(2, true, pressed);
            break;
        case SDL_SCANCODE_DOWN:
            updateState(3, true, pressed);
            break;
        case SDL_SCANCODE_Z:
            updateState(0, false, pressed);
            break;
        case SDL_SCANCODE_X:
            updateState(1, false, pressed);
            break;
        case SDL_SCANCODE_SPACE:
            updateState(2, false, pressed);
            break;
        case SDL_SCANCODE_RETURN:
            updateState(3, false, pressed);
            break;
        default:
            break;
        }
    }
}

void Joypad::updateState(int bit, bool isDirection, bool Pressed) {
    uint8_t &targetGroup = isDirection ? DirectionKeys : ButtonKeys;
    uint8_t previusState = targetGroup;

    if (Pressed) {
        targetGroup &= ~(1 << bit);

        if ((previusState >> bit) & 1) {
            RequestInterrupt = true;
        }
    } else {
        targetGroup |= (1 << bit);
    }
}