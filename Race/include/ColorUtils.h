#pragma once

#include <Arduino.h>

namespace ColorUtils
{
    void SplitColor(uint32_t color32, uint8_t &r, uint8_t &g, uint8_t &b);
    void ToHTMLColor(uint32_t color32, char html[8]);
    uint32_t FromHTMLColor(const char *color);
    String toString(uint32_t color32);
};