#include "PixelArt.h"

// Exact 32x32 monochrome transcription of the supplied B&F bead pattern.
// Rows are MSB-first: bit 31 is x=0; set pixels use the foreground colour.
static const uint32_t kDefaultRows[PIXEL_ART_H] = {
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
  0xFFFFFFFF,0xF80000FF,0xF800003F,0xF800001F,
  0xF800001F,0xF800000F,0xF81FFC0F,0xF81FFC0F,
  0xF81FFC0F,0xF81FFC0F,0xF81F000F,0xF81F001F,
  0xF81F001F,0xF81F001F,0xF81F000F,0xF81FFC0F,
  0xFC1FFC0F,0xFC1FFC0F,0xFE1FFC0F,0xFE1F000F,
  0xFF1F001F,0xFF9F001F,0xFFFF003F,0xFFFF00FF,
  0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF
};

void pixelArtDefaultHex(String& out) {
  static const char kHex[] = "0123456789abcdef";
  out = "";
  out.reserve(PIXEL_ART_HEX_LEN);
  for (uint8_t y = 0; y < PIXEL_ART_H; ++y) {
    uint32_t row = kDefaultRows[y];
    for (int shift = 28; shift >= 0; shift -= 4) out += kHex[(row >> shift) & 0xF];
  }
}

bool pixelArtHexValid(const String& hex) {
  if (hex.length() != PIXEL_ART_HEX_LEN) return false;
  for (size_t i = 0; i < hex.length(); ++i) {
    char c = hex[i];
    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
          (c >= 'A' && c <= 'F'))) return false;
  }
  return true;
}

static uint8_t nibble(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  return c - 'A' + 10;
}

bool pixelArtDecode(const String& hex, uint8_t out[PIXEL_ART_BYTES]) {
  if (!pixelArtHexValid(hex)) return false;
  for (size_t i = 0; i < PIXEL_ART_BYTES; ++i)
    out[i] = (uint8_t)((nibble(hex[i * 2]) << 4) | nibble(hex[i * 2 + 1]));
  return true;
}

uint16_t pixelArtRgb565(uint32_t c) {
  return (uint16_t)((((c >> 16) & 0xFF) >> 3) << 11 |
                    (((c >> 8)  & 0xFF) >> 2) << 5  |
                    ((c & 0xFF) >> 3));
}
