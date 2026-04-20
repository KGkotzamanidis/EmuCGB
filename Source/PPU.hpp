/*
 *An Gameboy and GameboyColor emulation with project name EmuCGB
 *Copyright (C) <2025>  <KGkotzamanidis>
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
#ifndef _PPU_H_
#define _PPU_H_

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <array>
#include <cstdint>
#include <cstring>

#include "EmulationUtils.hpp"
#include "HWRegisters.hpp"
#include "Interrupts.hpp"

// =============================================================================
// Screen geometry
// =============================================================================
static constexpr int GB_W = 160;
static constexpr int GB_H = 144;
static constexpr int GB_VBLANK = 10;
static constexpr int GB_LINES = GB_H + GB_VBLANK; // 154

// =============================================================================
// PPU dot-clock budget per scanline
// =============================================================================
static constexpr int DOTS_MODE2 = 80;
static constexpr int DOTS_MODE3 = 172;
static constexpr int DOTS_MODE0 = 204;
static constexpr int DOTS_LINE = 456;

// =============================================================================
// DMG green-screen palette (ARGB8888)
// =============================================================================
static constexpr uint32_t DMG_COLORS[4] = {
    0xFF9BBC0F, 0xFF8BAC0F, 0xFF306230, 0xFF0F380F};

// =============================================================================
// OAM sprite entry
// =============================================================================
struct OAMEntry {
    uint8_t y;
    uint8_t x;
    uint8_t tile;
    uint8_t flags;
};
static constexpr uint8_t SPR_PRIORITY = 1 << 7;
static constexpr uint8_t SPR_FLIP_Y = 1 << 6;
static constexpr uint8_t SPR_FLIP_X = 1 << 5;
static constexpr uint8_t SPR_DMG_PAL = 1 << 4;
static constexpr uint8_t SPR_CGB_BANK = 1 << 3;
static constexpr uint8_t SPR_CGB_PAL = 0x07;

// =============================================================================
// PPU
// =============================================================================
class PPU {
public:
    PPU(Interrupts &interrupts, bool cgbMode);
    ~PPU();

    // --- Main clock ---
    bool step(int cycles); // returns true when VBlank starts (new frame)

    // --- SDL3 ---
    bool initSDL(const char *title, int scale, const char *icon);
    bool setIcon(SDL_Window *window, const char *path);
    void destroySDL();
    void present();

    const uint32_t *framebuffer() const { return fb.data(); }

    // --- Memory interface (called by MMU) ---
    uint8_t readVRAM(uint16_t address);
    void writeVRAM(uint16_t address, uint8_t data);

    uint8_t readOAM(uint16_t address);
    void writeOAM(uint16_t address, uint8_t data);

    uint8_t readIO(uint16_t address);
    void writeIO(uint16_t address, uint8_t data);

    void startDMA(uint8_t sourcePage,
                  uint8_t (*readCallback)(uint16_t, void *),
                  void *ctx);

    // -------------------------------------------------------------------------
    // Public register mirror — read-only access for debuggers, test harnesses,
    // or any subsystem that needs to inspect PPU state without going via the MMU.
    // The PPU owns and writes these; external code must not write them directly.
    // -------------------------------------------------------------------------
    const PPURegisters &getRegisters() const { return Registers; }

private:
    Interrupts *interrupts = nullptr;
    bool cgbMode = false;

    // -------------------------------------------------------------------------
    // All LCD hardware registers live here — mirrors what PPURegisters defines.
    // The PPU reads and writes Registers.LCDC, Registers.STAT, etc. directly.
    // -------------------------------------------------------------------------
    PPURegisters Registers;

    // SDL3
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
    int scale = 3;

    // Framebuffer — 160×144 ARGB8888
    std::array<uint32_t, GB_W * GB_H> fb{};

    // VRAM: 2 banks × 8 KiB
    uint8_t VRAM[2][0x2000] = {};

    // OAM: 40 sprites × 4 bytes
    uint8_t OAM[0xA0] = {};

    // Timing (not stored in PPURegisters — internal PPU state only)
    int dotCounter = 0;
    int LY = 0; // shadow of Registers.LY for fast access

    // Window line counter (resets each VBlank)
    int windowLine = 0;

    // Mode
    enum class Mode : uint8_t { HBlank = 0,
                                VBlank = 1,
                                OAMScan = 2,
                                Drawing = 3 };
    Mode currentMode = Mode::OAMScan;

    // Helpers
    void setMode(Mode m);
    void requestInterrupt(uint8_t bit);
    void renderScanline(int ly);
    void renderBackground(int ly);
    void renderWindow(int ly);
    void renderSprites(int ly);

    uint32_t dmgColor(uint8_t palette, uint8_t colorId) const;
    uint32_t cgbColor(const uint8_t *palData, uint8_t palIndex,
                      uint8_t colorId) const;

    void fetchTileRow(uint16_t tileBase, int tileIndex, int row,
                      uint8_t &lo, uint8_t &hi, int bank = 0) const;

    // LCDC bit helpers — delegate to PPURegisters helpers
    bool lcdOn() const { return Registers.lcdEnabled(); }
    bool winMapHigh() const { return Registers.winMapHigh(); }
    bool winEnabled() const { return Registers.winEnabled(); }
    bool tilesSigned() const { return Registers.tilesSigned(); }
    bool bgMapHigh() const { return Registers.bgMapHigh(); }
    bool spriteTall() const { return Registers.spriteTall(); }
    bool spritesOn() const { return Registers.spritesEnabled(); }
    bool bgWinEnabled() const { return Registers.bgWinEnabled(); }
};

#endif // _PPU_H_