#pragma once

#include <screen/screen.h>
#include <string>
#include <cstdint>

class Font {
public:
    static const int CHAR_WIDTH = 8;
    static const int CHAR_HEIGHT = 16;
    static int renderScale;
    static int letterSpacing;

    static void drawChar(screen_context_t ctx, screen_buffer_t buf,
                         int x, int y, unsigned char ch, int color);
    static void drawString(screen_context_t ctx, screen_buffer_t buf,
                           int x, int y, const std::string& text, int color);
    static int stringWidth(const std::string& text);
    static int stringHeight();

private:
    static const uint8_t* getCharData(unsigned char ch);
};
