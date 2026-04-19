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
#include "PPU.h"
#include <cstdio>

// =============================================================================
// HOW THE PPU WORKS — complete explanation
// =============================================================================
//
// The Game Boy screen is 160×144 pixels.  The PPU draws it one scanline at a
// time, from left to right, top to bottom — exactly like a CRT television.
//
// TIME MODEL
// ----------
// The PPU runs in lock-step with the CPU.  Both are clocked at 4,194,304 Hz.
// One CPU T-cycle = one PPU "dot".  Each scanline takes exactly 456 dots.
// There are 154 scanlines (144 visible + 10 VBlank), so a full frame takes:
//
//   154 × 456 = 70,224 dots ≈ 59.7 frames per second
//
// MODES PER SCANLINE (lines 0-143)
// ---------------------------------
//  ┌──────────────────────────────────────────────────────────────┐
//  │ Mode 2: OAM Scan  │ Mode 3: Drawing   │ Mode 0: HBlank       │
//  │   80 dots         │   172 dots        │   204 dots           │
//  └──────────────────────────────────────────────────────────────┘
//  Total: 456 dots
//
//  Mode 2 — OAM Scan (80 dots):
//    The PPU scans the 40 OAM entries looking for sprites whose Y range
//    covers the current scanline.  It collects up to 10.  CPU cannot read OAM.
//
//  Mode 3 — Drawing (172 dots minimum):
//    The PPU fetches tile data and pushes pixels into the framebuffer.
//    CPU cannot access VRAM or OAM during this mode.
//
//  Mode 0 — HBlank (204 dots):
//    The PPU is idle.  CPU can freely access VRAM and OAM.
//    A Registers.STAT interrupt can be requested here.
//
// VBLANK (lines 144-153)
// -----------------------
//  Mode 1 — VBlank:
//    10 complete lines × 456 dots = 4,560 dots.
//    The PPU is idle for the whole period.  CPU can freely access all memory.
//    A VBlank interrupt is requested at the start (line 144).
//    This is the window to upload new tile data, palettes, etc.
//
// LAYERS (drawn in order, back to front)
// ----------------------------------------
//  1. Background — 32×32 tile map, scrolled by Registers.SCX/Registers.SCY, wraps around.
//  2. Window     — second tile layer, drawn over BG from (Registers.WX-7, Registers.WY).
//  3. Sprites    — up to 10 per line, 8×8 or 8×16 pixels.
//
// TILE DATA FORMAT
// -----------------
//  Each tile is 8×8 pixels, stored as 16 bytes (2 bytes per row).
//  The two bytes for a row are called the low bitplane (lo) and
//  high bitplane (hi).  For each pixel column bit b (7=left, 0=right):
//
//    colorId = ((lo >> b) & 1) | (((hi >> b) & 1) << 1)   → 0, 1, 2, or 3
//
//  The colorId is then looked up in the palette register to get a shade.
//
// PALETTE (DMG)
// -------------
//  Registers.BGP (0xFF47): 8-bit register, 4×2-bit shade values.
//    bits 7-6 → shade for colorId 3
//    bits 5-4 → shade for colorId 2
//    bits 3-2 → shade for colorId 1
//    bits 1-0 → shade for colorId 0
//  Shade 0 = white, shade 3 = black.
//
// PALETTE (CGB)
// -------------
//  8 BG palettes and 8 OBJ palettes, each with 4 colours.
//  Each colour = 2 bytes (RGB555 little-endian).
//  Stored in Registers.BGPD[64] and Registers.OBPD[64].
//
// TILE MAP ADDRESSES
// -------------------
//  Registers.LCDC bit 3 selects BG tile map:   0→0x9800, 1→0x9C00
//  Registers.LCDC bit 6 selects Window map:    0→0x9800, 1→0x9C00
//  Registers.LCDC bit 4 selects tile data:     0→0x8800 (signed index), 1→0x8000 (unsigned)
//
// =============================================================================

// =============================================================================
// Construction / Destruction
// =============================================================================
PPU::PPU(Interrupts &interrupts, bool cgbMode)
    : interrupts(&interrupts), cgbMode(cgbMode) {
    std::memset(VRAM, 0, sizeof(VRAM));
    std::memset(OAM, 0, sizeof(OAM));
    fb.fill(DMG_COLORS[0]);
    setMode(Mode::OAMScan);
    std::printf("-=PPU class initialized (mode: %s)=-\n", cgbMode ? "CGB" : "DMG");
}

PPU::~PPU() {
    destroySDL();
}

// =============================================================================
// SDL3 — window, renderer, streaming texture
// =============================================================================
bool PPU::initSDL(const char *title, int scaleFactor, const char *icon) {
    scale = scaleFactor;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::printf("PPU: SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }

    // Create a window at the scaled resolution.
    // SDL_SetRenderLogicalPresentation will handle the upscale internally,
    // so we always render at 160×144 regardless of the window size.
    window = SDL_CreateWindow(title,
                              GB_W * scale, GB_H * scale, 0);
    if (!window) {
        std::printf("PPU: SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::printf("PPU: SDL_CreateRenderer failed: %s\n", SDL_GetError());
        return false;
    }

    // Tell SDL the logical size is 160×144. It will scale to fit the window
    // automatically, keeping the correct aspect ratio.
    SDL_SetRenderLogicalPresentation(renderer, GB_W, GB_H,
                                     SDL_LOGICAL_PRESENTATION_STRETCH);

    // Streaming texture: we write ARGB8888 pixels into it every frame.
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                GB_W, GB_H);
    if (!texture) {
        std::printf("PPU: SDL_CreateTexture failed: %s\n", SDL_GetError());
        return false;
    }

    // Nearest-neighbour keeps pixels sharp when scaled up.
    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
    setIcon(window, icon);
    std::printf("PPU: SDL3 window ready (%dx%d, scale=%d)\n",
                GB_W, GB_H, scale);
    return true;
}

bool PPU::setIcon(SDL_Window *window, const char *path) {
    SDL_Surface *icon = IMG_Load(path);
    SDL_SetWindowIcon(window, icon);

    return true;
}

void PPU::destroySDL() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
}

// present() — uploads the framebuffer to the GPU and shows it on screen.
// Call this once per VBlank (when step() returns true).
void PPU::present() {
    if (!renderer || !texture)
        return;

    // Upload our CPU-side pixel array into the SDL streaming texture.
    SDL_UpdateTexture(texture, nullptr,
                      fb.data(), GB_W * static_cast<int>(sizeof(uint32_t)));

    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

// =============================================================================
// Memory interface — MMU calls these to replace its current stubs
// =============================================================================

// --- VRAM 0x8000-0x9FFF ------------------------------------------------------
// In CGB mode the active bank is selected by Registers.VBK (0xFF4F).
uint8_t PPU::readVRAM(uint16_t address) {
    // During Mode 3 (Drawing) VRAM is locked — returns 0xFF on real hardware.
    // We skip that restriction here for simplicity (most games don't care).
    return VRAM[Registers.VBK & 1][address - 0x8000];
}

void PPU::writeVRAM(uint16_t address, uint8_t data) {
    VRAM[Registers.VBK & 1][address - 0x8000] = data;
}

// --- OAM 0xFE00-0xFE9F -------------------------------------------------------
uint8_t PPU::readOAM(uint16_t address) {
    return OAM[address - 0xFE00];
}

void PPU::writeOAM(uint16_t address, uint8_t data) {
    OAM[address - 0xFE00] = data;
}

// --- LCD I/O registers -------------------------------------------------------
uint8_t PPU::readIO(uint16_t address) {
    switch (address) {
    case LCDCaddress:
        return Registers.LCDC;
    case STATaddress:
        return Registers.STAT | 0x80; // bit 7 always reads 1
    case SCYaddress:
        return Registers.SCY;
    case SCXaddress:
        return Registers.SCX;
    case LYaddress:
        return static_cast<uint8_t>(LY);
    case LYCaddress:
        return Registers.LYC;
    case BGPaddress:
        return Registers.BGP;
    case OBP0address:
        return Registers.OBP0;
    case OBP1address:
        return Registers.OBP1;
    case WYaddress:
        return Registers.WY;
    case WXaddress:
        return Registers.WX;
    case VBKaddress:
        return cgbMode ? (Registers.VBK | 0xFE) : 0xFF;
    case BGPIaddress:
        return cgbMode ? Registers.BGPI : 0xFF;
    case BGPDaddress:
        return cgbMode ? Registers.BGPD[Registers.BGPI & 0x3F] : 0xFF;
    case OBPIaddress:
        return cgbMode ? Registers.OBPI : 0xFF;
    case OBPDaddress:
        return cgbMode ? Registers.OBPD[Registers.OBPI & 0x3F] : 0xFF;
    default:
        return 0xFF;
    }
}

void PPU::writeIO(uint16_t address, uint8_t data) {
    switch (address) {

    case LCDCaddress:
        // Turning the LCD off resets the PPU to a known state and clears the screen.
        if ((Registers.LCDC & 0x80) && !(data & 0x80)) {
            LY = 0;
            Registers.LY = 0;
            dotCounter = 0;
            setMode(Mode::HBlank);
            fb.fill(DMG_COLORS[0]);
        }
        Registers.LCDC = data;
        break;

    case STATaddress:
        // Bits 0-2 are read-only (written by PPU), bits 3-6 are R/W.
        Registers.STAT = (Registers.STAT & 0x07) | (data & 0x78);
        break;

    case SCYaddress:
        Registers.SCY = data;
        break;
    case SCXaddress:
        Registers.SCX = data;
        break;
    case LYaddress:
        break; // LY is read-only

    case LYCaddress:
        Registers.LYC = data;
        // Re-evaluate the coincidence flag immediately after LYC changes.
        if (LY == Registers.LYC) {
            Registers.setCoincidence(true);
            if (Registers.STAT & coincidence)
                requestInterrupt(1);
        } else {
            Registers.setCoincidence(false);
        }
        break;

    case BGPaddress:
        Registers.BGP = data;
        break;
    case OBP0address:
        Registers.OBP0 = data;
        break;
    case OBP1address:
        Registers.OBP1 = data;
        break;
    case WYaddress:
        Registers.WY = data;
        break;
    case WXaddress:
        Registers.WX = data;
        break;

    case VBKaddress:
        if (cgbMode)
            Registers.VBK = data & 0x01;
        break;

    case BGPIaddress:
        if (cgbMode)
            Registers.BGPI = data;
        break;
    case BGPDaddress:
        if (cgbMode) {
            Registers.BGPD[Registers.BGPI & 0x3F] = data;
            if (Registers.BGPI & 0x80) // auto-increment flag
                Registers.BGPI = 0x80 | ((Registers.BGPI + 1) & 0x3F);
        }
        break;
    case OBPIaddress:
        if (cgbMode)
            Registers.OBPI = data;
        break;
    case OBPDaddress:
        if (cgbMode) {
            Registers.OBPD[Registers.OBPI & 0x3F] = data;
            if (Registers.OBPI & 0x80)
                Registers.OBPI = 0x80 | ((Registers.OBPI + 1) & 0x3F);
        }
        break;

    default:
        break;
    }
}

// --- OAM DMA -----------------------------------------------------------------
// The CPU writes a page number to 0xFF46 (e.g. 0xC0 means copy from 0xC000).
// The PPU copies 160 bytes into OAM.
// Because the MMU owns the actual read path, it passes a callback here.
void PPU::startDMA(uint8_t sourcePage,
                   uint8_t (*readCallback)(uint16_t, void *),
                   void *ctx) {
    uint16_t base = static_cast<uint16_t>(sourcePage) << 8;
    for (int i = 0; i < 0xA0; i++) {
        OAM[i] = readCallback(base + i, ctx);
    }
}

// =============================================================================
// Interrupt helper
// =============================================================================
void PPU::requestInterrupt(uint8_t bit) {
    interrupts->Registers.IF |= (1u << bit);
}

// =============================================================================
// Mode transition
// =============================================================================
void PPU::setMode(Mode m) {
    currentMode = m;
    // Write the mode into STAT bits 0-1
    Registers.setMode(static_cast<uint8_t>(m));

    // Fire STAT interrupts for modes that have an enable bit in STAT
    switch (m) {
    case Mode::HBlank:
        if (Registers.STAT & mode0)
            requestInterrupt(1); // STAT source: HBlank
        break;
    case Mode::VBlank:
        requestInterrupt(0); // VBlank interrupt (bit 0)
        if (Registers.STAT & mode1)
            requestInterrupt(1); // STAT source: VBlank
        break;
    case Mode::OAMScan:
        if (Registers.STAT & mode2)
            requestInterrupt(1); // STAT source: OAM
        break;
    case Mode::Drawing:
        break;
    }
}

// =============================================================================
// step(cycles) — the heart of the PPU
//
// This is called after every CPU instruction with the number of T-cycles
// that instruction consumed.  The PPU advances its dot counter and
// transitions between modes at the correct boundaries.
// =============================================================================
bool PPU::step(int cycles) {
    if (!lcdOn())
        return false;

    bool newFrame = false;
    dotCounter += cycles;

    switch (currentMode) {

    // -------------------------------------------------------------------------
    // Mode 2 — OAM Scan (80 dots)
    // The PPU scans OAM for sprites that overlap this scanline.
    // We do the actual work in renderSprites() during Mode 3.
    // -------------------------------------------------------------------------
    case Mode::OAMScan:
        if (dotCounter >= DOTS_MODE2) {
            dotCounter -= DOTS_MODE2;
            setMode(Mode::Drawing);
        }
        break;

    // -------------------------------------------------------------------------
    // Mode 3 — Drawing (172 dots)
    // At the end of this mode we render the full scanline into the framebuffer.
    // -------------------------------------------------------------------------
    case Mode::Drawing:
        if (dotCounter >= DOTS_MODE3) {
            dotCounter -= DOTS_MODE3;
            renderScanline(LY); // draw pixels for this scanline
            setMode(Mode::HBlank);
        }
        break;

    // -------------------------------------------------------------------------
    // Mode 0 — HBlank (204 dots)
    // At the end we advance to the next scanline.
    // -------------------------------------------------------------------------
    case Mode::HBlank:
        if (dotCounter >= DOTS_MODE0) {
            dotCounter -= DOTS_MODE0;
            LY++;
            Registers.LY = static_cast<uint8_t>(LY); // keep struct in sync

            // LYC coincidence check
            if (LY == Registers.LYC) {
                Registers.setCoincidence(true);
                if (Registers.STAT & coincidence)
                    requestInterrupt(1);
            } else {
                Registers.setCoincidence(false);
            }

            if (LY == GB_H) {
                windowLine = 0;
                setMode(Mode::VBlank);
                newFrame = true;
            } else {
                setMode(Mode::OAMScan);
            }
        }
        break;

    // -------------------------------------------------------------------------
    // Mode 1 — VBlank (10 lines × 456 dots)
    // We count through lines 144-153, then wrap back to line 0.
    // -------------------------------------------------------------------------
    case Mode::VBlank:
        if (dotCounter >= DOTS_LINE) {
            dotCounter -= DOTS_LINE;
            LY++;
            Registers.LY = static_cast<uint8_t>(LY); // keep struct in sync

            if (LY == Registers.LYC) {
                Registers.setCoincidence(true);
                if (Registers.STAT & coincidence)
                    requestInterrupt(1);
            } else {
                Registers.setCoincidence(false);
            }

            if (LY >= GB_LINES) {
                // End of VBlank — restart from line 0
                LY = 0;
                windowLine = 0;
                Registers.LY = 0;
                setMode(Mode::OAMScan);

                // Re-check LYC coincidence for line 0
                if (LY == Registers.LYC) {
                    Registers.setCoincidence(true);
                    if (Registers.STAT & coincidence)
                        requestInterrupt(1);
                } else {
                    Registers.setCoincidence(false);
                }
            }
        }
        break;
    }

    return newFrame;
}

// =============================================================================
// Tile data fetch
//
// Fetches the two bitplane bytes (lo, hi) for a given row of a tile.
//
//  tileBase  = 0x8000 → unsigned tile indices (Registers.LCDC bit 4 = 1)
//  tileBase  = 0x8800 → signed tile indices   (Registers.LCDC bit 4 = 0)
//  tileIndex = 0-255 (unsigned) or -128..127 (signed, treated as int8_t)
//  row       = 0-7 for 8×8, 0-15 for 8×16 sprites
//  lo, hi    = output: the two bitplane bytes
//  bank      = 0 or 1 (VRAM bank; CGB only, always 0 for DMG)
// =============================================================================
void PPU::fetchTileRow(uint16_t tileBase, int tileIndex, int row,
                       uint8_t &lo, uint8_t &hi, int bank) const {
    uint16_t addr;
    if (tileBase == 0x8800) {
        // Signed addressing: tile index 0 lives at 0x9000
        int8_t si = static_cast<int8_t>(tileIndex);
        addr = static_cast<uint16_t>(0x9000 + si * 16 + row * 2);
    } else {
        // Unsigned addressing: tile index 0 lives at 0x8000
        addr = static_cast<uint16_t>(0x8000 + tileIndex * 16 + row * 2);
    }
    lo = VRAM[bank & 1][addr - 0x8000];
    hi = VRAM[bank & 1][addr - 0x8000 + 1];
}

// =============================================================================
// Palette helpers
// =============================================================================

// DMG: extract a 2-bit shade from a palette register and map to ARGB8888
uint32_t PPU::dmgColor(uint8_t palette, uint8_t colorId) const {
    uint8_t shade = (palette >> (colorId * 2)) & 0x03;
    return DMG_COLORS[shade];
}

// CGB: read a RGB555 colour from Registers.BGPD or Registers.OBPD and convert to ARGB8888
uint32_t PPU::cgbColor(const uint8_t *palData, uint8_t palIndex,
                       uint8_t colorId) const {
    int off = palIndex * 8 + colorId * 2;
    uint16_t raw = palData[off] | (palData[off + 1] << 8);

    // RGB555 → RGB888: each channel is 5 bits, scale to 8 bits
    uint8_t r5 = (raw >> 0) & 0x1F;
    uint8_t g5 = (raw >> 5) & 0x1F;
    uint8_t b5 = (raw >> 10) & 0x1F;

    uint8_t r8 = (r5 << 3) | (r5 >> 2);
    uint8_t g8 = (g5 << 3) | (g5 >> 2);
    uint8_t b8 = (b5 << 3) | (b5 >> 2);

    return 0xFF000000u | (r8 << 16) | (g8 << 8) | b8;
}

// =============================================================================
// renderScanline — entry point called once per line at the end of Mode 3
// =============================================================================
void PPU::renderScanline(int ly) {
    if (ly < 0 || ly >= GB_H)
        return;

    // Fill the line with colour 0 of the BG palette as the default background
    uint32_t *line = &fb[ly * GB_W];
    uint32_t bgc0 = cgbMode ? cgbColor(Registers.BGPD, 0, 0) : dmgColor(Registers.BGP, 0);
    for (int x = 0; x < GB_W; x++)
        line[x] = bgc0;

    // Draw layers back-to-front
    if (bgWinEnabled() || cgbMode) {
        renderBackground(ly);
        if (winEnabled())
            renderWindow(ly);
    }
    if (spritesOn()) {
        renderSprites(ly);
    }
}

// =============================================================================
// Background rendering
//
// The BG is a 32×32 grid of 8×8 tiles (256×256 pixels total).
// It is scrolled by Registers.SCX (horizontal) and Registers.SCY (vertical).
// It wraps around — scrolling past edge 255 wraps back to 0.
// =============================================================================
void PPU::renderBackground(int ly) {
    uint16_t mapBase = bgMapHigh() ? 0x9C00 : 0x9800;    // tile map address
    uint16_t dataBase = tilesSigned() ? 0x8800 : 0x8000; // tile data address

    // Which pixel row of the BG we're rendering (0-255, wraps)
    uint8_t bgY = static_cast<uint8_t>(ly + Registers.SCY);
    int tileRow = bgY / 8;  // which row of tiles in the 32×32 map
    int pixelRow = bgY % 8; // which row of pixels inside the tile

    for (int sx = 0; sx < GB_W; sx++) {
        uint8_t bgX = static_cast<uint8_t>(sx + Registers.SCX); // wrapping X
        int tileCol = bgX / 8;
        int pixelCol = 7 - (bgX % 8); // bit 7 = leftmost pixel in a byte

        // Look up the tile index in the tile map
        uint16_t mapAddr = mapBase + tileRow * 32 + tileCol;
        uint8_t tileIdx = VRAM[0][mapAddr - 0x8000];

        // CGB: read tile attributes from VRAM bank 1
        uint8_t attr = cgbMode ? VRAM[1][mapAddr - 0x8000] : 0x00;
        int vramBank = cgbMode ? ((attr >> 3) & 1) : 0;
        bool flipX = cgbMode && (attr & 0x20);
        bool flipY = cgbMode && (attr & 0x40);
        uint8_t cgbPal = cgbMode ? (attr & 0x07) : 0;
        bool bgPrio = cgbMode && (attr & 0x80);

        // Apply tile flip
        int row = flipY ? (7 - pixelRow) : pixelRow;
        int col = flipX ? (bgX % 8) : pixelCol;

        // Fetch the two bitplane bytes for this tile row
        uint8_t lo, hi;
        fetchTileRow(dataBase, tileIdx, row, lo, hi, vramBank);

        // Combine bitplanes to get the 2-bit colour index (0-3)
        uint8_t colorId = ((lo >> col) & 1) | (((hi >> col) & 1) << 1);

        uint32_t color;
        if (cgbMode) {
            color = cgbColor(Registers.BGPD, cgbPal, colorId);
        } else {
            color = dmgColor(Registers.BGP, colorId);
        }

        fb[ly * GB_W + sx] = color;

        // Store BG-to-OBJ priority in the alpha channel high bit
        // so the sprite renderer can check it without a separate buffer.
        if (bgPrio && colorId != 0)
            fb[ly * GB_W + sx] |= 0x01000000u;
    }
}

// =============================================================================
// Window rendering
//
// The Window is a second tile layer drawn on top of the BG.
// It starts at screen position (Registers.WX-7, Registers.WY).
// Unlike the BG it does NOT scroll and does NOT wrap.
// =============================================================================
void PPU::renderWindow(int ly) {
    if (ly < Registers.WY)
        return; // window hasn't started yet this frame
    int wx = static_cast<int>(Registers.WX) - 7;
    if (wx >= GB_W)
        return; // window is off the right edge

    uint16_t mapBase = winMapHigh() ? 0x9C00 : 0x9800;
    uint16_t dataBase = tilesSigned() ? 0x8800 : 0x8000;

    int pixelRow = windowLine % 8;
    int tileRow = windowLine / 8;
    windowLine++; // advance for the next scanline that renders the window

    int startX = (wx < 0) ? 0 : wx;

    for (int sx = startX; sx < GB_W; sx++) {
        int winX = sx - wx;
        int tileCol = winX / 8;
        int pixelCol = 7 - (winX % 8);

        uint16_t mapAddr = mapBase + tileRow * 32 + tileCol;
        uint8_t tileIdx = VRAM[0][mapAddr - 0x8000];

        uint8_t attr = cgbMode ? VRAM[1][mapAddr - 0x8000] : 0x00;
        int vramBank = cgbMode ? ((attr >> 3) & 1) : 0;
        bool flipX = cgbMode && (attr & 0x20);
        bool flipY = cgbMode && (attr & 0x40);
        uint8_t cgbPal = cgbMode ? (attr & 0x07) : 0;
        bool bgPrio = cgbMode && (attr & 0x80);

        int row = flipY ? (7 - pixelRow) : pixelRow;
        int col = flipX ? (winX % 8) : pixelCol;

        uint8_t lo, hi;
        fetchTileRow(dataBase, tileIdx, row, lo, hi, vramBank);

        uint8_t colorId = ((lo >> col) & 1) | (((hi >> col) & 1) << 1);

        uint32_t color;
        if (cgbMode) {
            color = cgbColor(Registers.BGPD, cgbPal, colorId);
        } else {
            color = dmgColor(Registers.BGP, colorId);
        }

        fb[ly * GB_W + sx] = color;
        if (bgPrio && colorId != 0)
            fb[ly * GB_W + sx] |= 0x01000000u;
    }
}

// =============================================================================
// Sprite rendering
//
// Sprites are stored in OAM (Object Attribute Memory) at 0xFE00-0xFE9F.
// There are 40 sprites; at most 10 can appear on one scanline.
//
// Priority rule (DMG):
//   Lower X coordinate → drawn on top.
//   Equal X → lower OAM index wins.
//
// Sprite colour 0 is always transparent.
// =============================================================================
void PPU::renderSprites(int ly) {
    int sh = spriteTall() ? 16 : 8; // sprite height: 8×8 or 8×16

    // --- Collect visible sprites (up to 10) ---
    struct SprInfo {
        int idx;
        int x;
    };
    SprInfo visible[10];
    int count = 0;

    for (int i = 0; i < 40 && count < 10; i++) {
        const OAMEntry *s = reinterpret_cast<const OAMEntry *>(&OAM[i * 4]);
        int sy = static_cast<int>(s->y) - 16;
        if (ly >= sy && ly < sy + sh)
            visible[count++] = {i, static_cast<int>(s->x)};
    }

    // --- Sort by X (insertion sort — max 10 items, very fast) ---
    for (int i = 1; i < count; i++) {
        SprInfo key = visible[i];
        int j = i - 1;
        while (j >= 0 && visible[j].x > key.x) {
            visible[j + 1] = visible[j];
            j--;
        }
        visible[j + 1] = key;
    }

    // --- Draw in reverse order so highest-priority sprite wins ---
    for (int s = count - 1; s >= 0; s--) {
        const OAMEntry *sp =
            reinterpret_cast<const OAMEntry *>(&OAM[visible[s].idx * 4]);

        int sy = static_cast<int>(sp->y) - 16;
        int sx = static_cast<int>(sp->x) - 8;
        bool flipX = sp->flags & SPR_FLIP_X;
        bool flipY = sp->flags & SPR_FLIP_Y;
        bool behind = sp->flags & SPR_PRIORITY; // sprite behind BG colours 1-3

        int row = ly - sy;
        if (flipY)
            row = sh - 1 - row;

        // For 8×16, bit 0 of the tile index is ignored (top and bottom tiles
        // are consecutive: tile N and tile N+1)
        uint8_t tileIdx = sp->tile;
        if (sh == 16)
            tileIdx &= 0xFE;

        int vramBank = cgbMode ? ((sp->flags & SPR_CGB_BANK) ? 1 : 0) : 0;
        uint8_t cgbPal = cgbMode ? (sp->flags & SPR_CGB_PAL) : 0;
        uint8_t dmgPal = (sp->flags & SPR_DMG_PAL) ? Registers.OBP1 : Registers.OBP0;

        uint8_t lo, hi;
        fetchTileRow(0x8000, tileIdx, row, lo, hi, vramBank);

        for (int px = 0; px < 8; px++) {
            int screenX = sx + px;
            if (screenX < 0 || screenX >= GB_W)
                continue;

            int bit = flipX ? px : (7 - px);
            uint8_t colorId = ((lo >> bit) & 1) | (((hi >> bit) & 1) << 1);

            if (colorId == 0)
                continue; // colour 0 is always transparent

            // Priority check: is this sprite behind BG colours 1-3?
            uint32_t existing = fb[ly * GB_W + screenX];
            bool bgHasPriority = (existing & 0x01000000u) != 0;

            if (behind || bgHasPriority) {
                // Only overwrite if the BG pixel is colour 0
                uint32_t bg0 = cgbMode ? cgbColor(Registers.BGPD, 0, 0) : dmgColor(Registers.BGP, 0);
                if ((existing & 0x00FFFFFFu) != (bg0 & 0x00FFFFFFu))
                    continue;
            }

            uint32_t color;
            if (cgbMode) {
                color = cgbColor(Registers.OBPD, cgbPal, colorId);
            } else {
                color = dmgColor(dmgPal, colorId);
            }

            fb[ly * GB_W + screenX] = color;
        }
    }

    // --- Strip the priority metadata from the alpha channel ---
    // (Restore full opaque alpha so SDL presents the pixels correctly)
    uint32_t *line = &fb[ly * GB_W];
    for (int x = 0; x < GB_W; x++)
        line[x] |= 0xFF000000u;
}