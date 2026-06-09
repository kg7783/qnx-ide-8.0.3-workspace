#include "StatsView.h"
#include "Font.h"
#include <cstdio>

StatsView::StatsView(Rect bounds)
    : Widget(bounds)
{
}

void StatsView::setStats(int c, int w) {
    correct = c;
    wrong = w;
    total = c + w;
}

void StatsView::draw(screen_context_t ctx, screen_buffer_t buf) {
    if (!visible) return;

    int x = bounds.x;
    int y = bounds.y;
    int w = bounds.w;
    int h = bounds.h;

    int bgColor = 0xFFE0E0E0;

    int bgRect[] = {
        SCREEN_BLIT_DESTINATION_X, x,
        SCREEN_BLIT_DESTINATION_Y, y,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, bgColor,
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bgRect);

    int barH = 8;
    int barY = y + h - barH - 4;
    int barX = x + 4;
    int barW = w - 8;

    int barBg[] = {
        SCREEN_BLIT_DESTINATION_X, barX,
        SCREEN_BLIT_DESTINATION_Y, barY,
        SCREEN_BLIT_DESTINATION_WIDTH, barW,
        SCREEN_BLIT_DESTINATION_HEIGHT, barH,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFCCCCCC),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, barBg);

    if (total > 0) {
        float ratio = (float)correct / total;
        int correctW = (int)(barW * ratio);
        if (correctW > 0) {
            int correctRect[] = {
                SCREEN_BLIT_DESTINATION_X, barX,
                SCREEN_BLIT_DESTINATION_Y, barY,
                SCREEN_BLIT_DESTINATION_WIDTH, correctW,
                SCREEN_BLIT_DESTINATION_HEIGHT, barH,
                SCREEN_BLIT_COLOR, static_cast<int>(0xFF228B22),
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, correctRect);
        }
    }

    char bufCorrect[16];
    snprintf(bufCorrect, sizeof(bufCorrect), "%d", correct);
    char bufWrong[16];
    snprintf(bufWrong, sizeof(bufWrong), "%d", wrong);
    char bufPct[16];
    if (total > 0) {
        int pct = (int)((float)correct / total * 100);
        snprintf(bufPct, sizeof(bufPct), "%d%%", pct);
    } else {
        snprintf(bufPct, sizeof(bufPct), "0%%");
    }

    Font::drawString(ctx, buf, x + 8, y + 2, bufCorrect, 0xFF228B22);
    Font::drawString(ctx, buf, x + 8 + Font::stringWidth(bufCorrect) + 4, y + 2, "O", 0xFF228B22);

    int rightX = x + w - Font::stringWidth(bufWrong) - Font::stringWidth("X") - 12;
    Font::drawString(ctx, buf, rightX, y + 2, bufWrong, 0xFFDC143C);
    Font::drawString(ctx, buf, rightX + Font::stringWidth(bufWrong) + 4, y + 2, "X", 0xFFDC143C);

    int cx = x + (w - Font::stringWidth(bufPct)) / 2;
    Font::drawString(ctx, buf, cx, y + 2, bufPct, 0xFF666666);
}
