#pragma once
#include "Mode.h"

class BrandScreen : public DisplayMode {
 public:
  const char* id() const override { return "brand"; }
  uint8_t modeConst() const override { return MODE_BRAND; }
  void begin(const Settings& s) override { invalidate(s); }
  void service(const Settings& s) override;
  void invalidate(const Settings& s) override { dirty_ = true; }
  void wake(const Settings& s) override { dirty_ = true; }
 private:
  void render(const Settings& s);
  bool dirty_ = true;
};

extern BrandScreen g_brandScreen;

