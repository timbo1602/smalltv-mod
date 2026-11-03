#pragma once
#include "Mode.h"

class ClockScreen : public DisplayMode {
 public:
  const char* id() const override { return "clock"; }
  uint8_t modeConst() const override { return MODE_CLOCK; }
  void begin(const Settings&) override { dirty_ = true; }
  void service(const Settings& s) override;
  void invalidate(const Settings&) override { dirty_ = true; }
  void wake(const Settings&) override { dirty_ = true; }
 private:
  bool dirty_ = true;
  String last_;
};

extern ClockScreen g_clockScreen;

