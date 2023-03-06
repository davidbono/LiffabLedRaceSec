#include "ColorUtils.h"

namespace ColorUtils
{
    void SplitColor(uint32_t color32, uint8_t &r, uint8_t &g, uint8_t &b)
    {
        r = ((color32 >> 16) & 0xFF);
        g = ((color32 >> 8) & 0xFF);
        b = (color32 & 0xFF);
    }

    void ToHTMLColor(uint32_t color32, char html[8])
    {
        uint8_t r, g, b;
        SplitColor(color32, r, g, b);
        sprintf(html, "#%02x%02x%02x", r, g, b);
    }

    uint32_t FromHTMLColor(const char *color)
    {
        //We want to ignore the starting character '#' from the HTML string, so we convert starting with color[1]
        return strtoul(&color[1], NULL, 16);
    }

    String toString(uint32_t color32)
    {
        uint8_t r,g,b;
        SplitColor(color32,r,g,b);
        return String(r) + "," + String(g) + "," + String(b);
    }
};