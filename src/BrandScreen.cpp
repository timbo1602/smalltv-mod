#include "BrandScreen.h"
#include "Gfx.h"
#include "PixelArt.h"
#include <Arduino_GFX_Library.h>

BrandScreen g_brandScreen;

void BrandScreen::render(const Settings& s) {
  Arduino_GFX* gfx = gfxDev();
  if (!gfx) return;
  uint8_t bits[PIXEL_ART_BYTES];
  String fallback;
  const String* source = &s.brand.bitmapHex;
  if (!pixelArtDecode(*source, bits)) {
    pixelArtDefaultHex(fallback);
    pixelArtDecode(fallback, bits);
  }
  uint32_t fg888 = s.brand.inverted ? s.brand.background : s.brand.foreground;
  uint32_t bg888 = s.brand.inverted ? s.brand.foreground : s.brand.background;
  uint16_t fg = gfxTint(pixelArtRgb565(fg888));
  uint16_t bg = gfxTint(pixelArtRgb565(bg888));
  gfx->fillScreen(bg);
  const uint8_t scale = s.brand.scale;
  for (uint8_t y = 0; y < PIXEL_ART_H; ++y) {
    for (uint8_t x = 0; x < PIXEL_ART_W; ++x) {
      const size_t bit = (size_t)y * PIXEL_ART_W + x;
      if (bits[bit >> 3] & (0x80 >> (bit & 7)))
        gfx->fillRect(s.brand.x + x * scale, s.brand.y + y * scale, scale, scale, fg);
    }
  }
}

void BrandScreen::service(const Settings& s) {
  if (!dirty_) return;
  render(s);
  dirty_ = false;
}

