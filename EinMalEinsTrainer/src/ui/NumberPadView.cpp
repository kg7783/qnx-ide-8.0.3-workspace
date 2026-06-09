#include "NumberPadView.h"
#include "Font.h"

NumberPadView::NumberPadView(Rect bounds)
    : Widget(bounds)
{
}

int NumberPadView::getButtonAt(int px, int py) const {
    int pad = 4;
    int spacing = 4;
    int btnW = (bounds.w - pad * 2 - spacing * (COLS - 1)) / COLS;
    int btnH = (bounds.h - pad * 2 - spacing * (ROWS - 1)) / ROWS;

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            int x = bounds.x + pad + col * (btnW + spacing);
            int y = bounds.y + pad + row * (btnH + spacing);
            if (px >= x && px < x + btnW && py >= y && py < y + btnH) {
                return row * COLS + col;
            }
        }
    }
    return -1;
}

void NumberPadView::draw(screen_context_t ctx, screen_buffer_t buf) {
    if (!visible) return;

    int pad = 4;
    int spacing = 4;
    int btnW = (bounds.w - pad * 2 - spacing * (COLS - 1)) / COLS;
    int btnH = (bounds.h - pad * 2 - spacing * (ROWS - 1)) / ROWS;

    for (int i = 0; i < NUM_BUTTONS; i++) {
        int row = i / COLS;
        int col = i % COLS;
        int x = bounds.x + pad + col * (btnW + spacing);
        int y = bounds.y + pad + row * (btnH + spacing);

        int bgColor = (i == lastPressed) ? 0xFFCCCCFF : 0xFFF0F0FA;

        int fill[] = {
            SCREEN_BLIT_DESTINATION_X, x,
            SCREEN_BLIT_DESTINATION_Y, y,
            SCREEN_BLIT_DESTINATION_WIDTH, btnW,
            SCREEN_BLIT_DESTINATION_HEIGHT, btnH,
            SCREEN_BLIT_COLOR, bgColor,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, fill);

        if (i == lastPressed) {
            int bdr[] = {
                SCREEN_BLIT_DESTINATION_X, x,
                SCREEN_BLIT_DESTINATION_Y, y,
                SCREEN_BLIT_DESTINATION_WIDTH, btnW,
                SCREEN_BLIT_DESTINATION_HEIGHT, 1,
                SCREEN_BLIT_COLOR, static_cast<int>(0xFF9999DD),
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, bdr);
            bdr[3] = y + btnH - 1;
            screen_fill(ctx, buf, bdr);
            int bdr2[] = {
                SCREEN_BLIT_DESTINATION_X, x,
                SCREEN_BLIT_DESTINATION_Y, y,
                SCREEN_BLIT_DESTINATION_WIDTH, 1,
                SCREEN_BLIT_DESTINATION_HEIGHT, btnH,
                SCREEN_BLIT_COLOR, static_cast<int>(0xFF9999DD),
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, bdr2);
            bdr2[1] = x + btnW - 1;
            screen_fill(ctx, buf, bdr2);
        }

        int tx = x + (btnW - Font::stringWidth(buttonLabels[i])) / 2;
        int ty = y + (btnH - Font::stringHeight()) / 2;
        Font::drawString(ctx, buf, tx, ty, buttonLabels[i], 0xFF000000);
    }
}

bool NumberPadView::handleEvent(int type, int x, int y, int key) {
    if (!visible || !enabled) return false;

    if (type == 0) {
        int idx = getButtonAt(x, y);
        if (idx >= 0) {
            lastPressed = idx;
            int val = buttonValues[idx];
            if (val >= 0 && onNumber) onNumber(val);
            else if (val == -1 && onDelete) onDelete();
            else if (val == -2 && onOk) onOk();
            return true;
        }
    } else if (type == 1) {
        lastPressed = -1;
        return false;
    }
    return false;
}
