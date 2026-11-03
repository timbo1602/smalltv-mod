#include "ScreenManager.h"
#include "config.h"

static int rankOf(const String& order, const char* id) {
  int rank = 0;
  int start = 0;
  while (start <= (int)order.length()) {
    int comma = order.indexOf(',', start);
    if (comma < 0) comma = order.length();
    if (order.substring(start, comma) == id) return rank;
    start = comma + 1;
    ++rank;
  }
  return 1000;
}

void screenManagerSort(DisplayMode** modes, size_t count, const Settings& s) {
  for (size_t i = 1; i < count; ++i) {
    DisplayMode* moving = modes[i];
    int movingRank = rankOf(s.screens.order, moving->id());
    size_t j = i;
    while (j > 0 && rankOf(s.screens.order, modes[j - 1]->id()) > movingRank) {
      modes[j] = modes[j - 1];
      --j;
    }
    modes[j] = moving;
  }
}

bool screenManagerEnabled(const DisplayMode* m, const Settings& s) {
  switch (m->modeConst()) {
    case MODE_BRAND:  return s.brand.enabled;
    case MODE_STOCKS: return s.screens.tickerEnabled;
    case MODE_CLOCK:  return s.screens.clockEnabled;
    case MODE_USAGE:  return s.carouselUsage;
    case MODE_RADAR:  return s.carouselRadar;
    case MODE_HA:     return s.carouselHa;
    case MODE_THEME:  return s.carouselTheme;
    default:          return false;
  }
}

uint16_t screenManagerDuration(const DisplayMode* m, const Settings& s) {
  if (m->modeConst() == MODE_BRAND) return s.brand.durationSec;
  if (m->modeConst() == MODE_STOCKS) return s.ticker.rotateSec;
  return s.screens.defaultDurationSec;
}

