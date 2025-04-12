/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <Sat Apr 12 2025>  <KGkotzamanidis>
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
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <cmath>
#include <cstdint>
#include <iostream>

#include "BIOS.h"
#include "MMU.h"
#include "ROM.h"

float lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

void getRainbowColor(float time, uint8_t &r, uint8_t &g, uint8_t &b) {
    r = static_cast<uint8_t>((std::sin(time + 0.0f) * 0.5f + 0.5f) * 255);
    g = static_cast<uint8_t>((std::sin(time + 2.0f) * 0.5f + 0.5f) * 255);
    b = static_cast<uint8_t>((std::sin(time + 4.0f) * 0.5f + 0.5f) * 255);
}

int main(int argc, char *argv[]) {
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Surface*icon = nullptr;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("EmuCGB", 500, 500, SDL_WINDOW_VULKAN);
    renderer = SDL_CreateRenderer(window, NULL);
    icon = IMG_Load("../Resource/GkotzamBoy.png");

    SDL_SetWindowIcon(window,icon);

    float time = 0.0f;

    SDL_Event event;
    bool application_loop = true;
    bool application_test = true;

    BIOS bios;
    ROM rom;
    MMU mmu(bios, rom);

    while (application_loop) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                application_loop = false;
            }
            if (event.type == SDL_EVENT_KEY_DOWN) {
                application_test = false;
            }
        }

        if (application_test) {
            time += 0.030;
            uint8_t r, g, b;
            getRainbowColor(time, r, g, b);

            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
            SDL_Delay(16);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);

            bios.loadBIOS("../Resource/dmg0.bin");
            rom.loadROM("../Resource/cpu_test.gb");
            rom.info();
        }

        while (!application_test) {
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    std::printf("Program terminate\n");
    return 0;
}