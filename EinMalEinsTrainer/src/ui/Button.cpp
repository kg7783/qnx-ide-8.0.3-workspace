#include "Button.h"

Button::Button(Rect bounds, const std::string& text, std::function<void()> onClick)
    : Widget(bounds), label(text), onClick(onClick)
{
}

void Button::setColors(Color bg, Color text, Color border) {
    bgColor = bg;
    textColor = text;
    borderColor = border;
}

void Button::draw(screen_context_t ctx, screen_buffer_t buf) {
    if (!visible) return;

    int color = pressed ? pressedBgColor.toARGB() : bgColor.toARGB();

    int bgRect[] = {
        SCREEN_BLIT_DESTINATION_X, bounds.x,
        SCREEN_BLIT_DESTINATION_Y, bounds.y,
        SCREEN_BLIT_DESTINATION_WIDTH, bounds.w,
        SCREEN_BLIT_DESTINATION_HEIGHT, bounds.h,
        SCREEN_BLIT_COLOR, color,
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bgRect);

    int bdrRect[] = {
        SCREEN_BLIT_DESTINATION_X, bounds.x,
        SCREEN_BLIT_DESTINATION_Y, bounds.y,
        SCREEN_BLIT_DESTINATION_WIDTH, bounds.w,
        SCREEN_BLIT_DESTINATION_HEIGHT, 1,
        SCREEN_BLIT_COLOR, borderColor.toARGB(),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bdrRect);
    bdrRect[3] = bounds.y + bounds.h - 1;
    screen_fill(ctx, buf, bdrRect);
    int bdrRect2[] = {
        SCREEN_BLIT_DESTINATION_X, bounds.x,
        SCREEN_BLIT_DESTINATION_Y, bounds.y,
        SCREEN_BLIT_DESTINATION_WIDTH, 1,
        SCREEN_BLIT_DESTINATION_HEIGHT, bounds.h,
        SCREEN_BLIT_COLOR, borderColor.toARGB(),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bdrRect2);
    bdrRect2[1] = bounds.x + bounds.w - 1;
    screen_fill(ctx, buf, bdrRect2);

    int tx = bounds.x + (bounds.w - Font::stringWidth(label)) / 2;
    int ty = bounds.y + (bounds.h - Font::stringHeight()) / 2;
    Font::drawString(ctx, buf, tx, ty, label, textColor.toARGB());
}

bool Button::handleEvent(int type, int x, int y, int key) {
    if (!visible || !enabled) return false;

    if (type == 0) {
        if (bounds.contains(x, y)) {
            pressed = true;
            return true;
        }
    } else if (type == 1) {
        if (pressed) {
            pressed = false;
            if (bounds.contains(x, y) && onClick) {
                onClick();
            }
            return true;
        }
        pressed = false;
    }
    return false;
}
