#pragma once
#include <Arduino.h>
#include "config.h"

// Compact 32x32 monochrome bitmap helpers. A set bit means foreground.
void pixelArtDefaultHex(String& out);
bool pixelArtHexValid(const String& hex);
bool pixelArtDecode(const String& hex, uint8_t out[PIXEL_ART_BYTES]);
uint16_t pixelArtRgb565(uint32_t rgb888);

