#pragma once
#include "Mode.h"

// Applies the persistent comma-separated module order to the registry in place.
void screenManagerSort(DisplayMode** modes, size_t count, const Settings& s);
bool screenManagerEnabled(const DisplayMode* mode, const Settings& s);
uint16_t screenManagerDuration(const DisplayMode* mode, const Settings& s);

