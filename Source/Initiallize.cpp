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
#include <SDL3/SDL.h>

#include "BIOS.hpp"
#include "EmulationUtils.hpp"
#include "Joypad.hpp"
#include "MMU.hpp"
#include "PPU.hpp"
#include "ROM.hpp"
#include "SM83.hpp"
#include "Timers.hpp"
#include "WRAM.hpp"

#define F_ROM "Resource/ROMS/tetris.gb"
#define F_BIOS "Resource/BIOS/dmg0.bin"
#define F_ICON "Resource/GkotzamBoy.png"

int main() {

    // --- Load BIOS and ROM ---------------------------------------------------
    BIOS bios;
    bios.loadBIOS(F_BIOS);

    ROM *rom = ROM::loadROM(F_ROM);
    bool CGBMode = (rom->receivingData(0x143) & 0x80) == 0x80;

    // --- Construct all subsystems -------------------------------------------
    Interrupts interrupts;
    Timers timers(interrupts);
    WRAM wram;
    Joypad joypad;

    // PPU must be constructed BEFORE MMU so connectPPU() can wire it in.
    // cgbMode comes from the cartridge header parsed by rom.loadROM().
    PPU ppu(interrupts, CGBMode);

    MMU mmu(bios, rom, interrupts, joypad, timers, wram);
    SM83 sm83(mmu);

    // Wire PPU into MMU — MUST happen before the first sm83.step(),
    // otherwise any VRAM/OAM/LCD access will hit a nullptr and crash.
    mmu.connectPPU(ppu);

    // --- Open the SDL3 window -----------------------------------------------
    if (!ppu.initSDL("EmuCGB", 3, F_ICON)) {
        std::printf("Fatal: could not open SDL3 window.\n");
        return 1;
    }

    // --- Emulation loop ------------------------------------------------------
    bool running = true;
    SDL_Event event;

    while (running) {

        // Poll SDL events so the window responds and can be closed
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
                std::printf("[!]SDL: Terminate Signal (ESC KEY PRESSED).\n");
                running = false;
            }
            // joypad.handlerEvent(event);
        }

        /*
        if (joypad.checkInterrupt()) {
            uint8_t if_flag = mmu.readByte(IFaddress);
            mmu.writeByte(IFaddress, if_flag | 0x10);
        }
            */

        // Step CPU and advance subsystems by the same number of T-cycles
        int cycles = sm83.step();
        timers.updateTimers(cycles);

        // Step PPU — returns true when VBlank starts (new frame is ready)
        if (ppu.step(cycles))
            ppu.present(); // upload framebuffer to screen
    }

    // --- Shutdown ------------------------------------------------------------
    ppu.destroySDL();

    // --- Save Rom Data -------------------------------------------------------
    rom->saveData();

    return 0;
}