#include "ClockScreen.h"
#include "Clock.h"
#include "Gfx.h"
#include "Net.h"
#include <Arduino_GFX_Library.h>

ClockScreen g_clockScreen;

void ClockScreen::service(const Settings&) {
  String now = clockTimeStr();
  if (!dirty_ && now == last_) return;
  last_ = now;
  dirty_ = false;
  Arduino_GFX* gfx = gfxDev();
  if (!gfx) return;
  gfx->fillScreen(C_BLACK);
  const char* time = now.length() ? now.c_str() : "--:--";
  gfxDrawCentered(time, 78, gfxFitSize(time, 228, 7), C_WHITE);
  gfxDrawCentered(clockSynced() ? "CLOCK" : "SYNCING", 152, 2,
                  clockSynced() ? C_GRAY : C_YELLOW);
  if (!clockSynced()) gfxDrawCentered(netIP().c_str(), 184, 2, C_DGRAY);
}

