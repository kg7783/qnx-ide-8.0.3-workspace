#include "HistoryScreen.h"
#include "Font.h"
#include <cstdio>

HistoryScreen::HistoryScreen(ScreenManager* mgr)
    : Screen(mgr), sessionMgr()
{
}

void HistoryScreen::onEnter() {
    sessions = sessionMgr.loadSessions();
    scrollOffset = 0;
}

void HistoryScreen::onLeave() {
}

void HistoryScreen::drawTabBar(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int tabW = w / TAB_COUNT;
    int tabH = 32;

    std::string tabLabels[TAB_COUNT] = {
        mgr->tr("tab_sessions"),
        mgr->tr("tab_statistics"),
        mgr->tr("tab_tables")
    };

    for (int i = 0; i < TAB_COUNT; i++) {
        int x = i * tabW;
        bool active = (i == (int)currentTab);
        int bgColor = active ? 0xFFFFFFFF : 0xFFE0E0E0;
        int textColor = active ? 0xFF0066CC : 0xFF666666;

        int fill[] = {
            SCREEN_BLIT_DESTINATION_X, x,
            SCREEN_BLIT_DESTINATION_Y, 0,
            SCREEN_BLIT_DESTINATION_WIDTH, tabW,
            SCREEN_BLIT_DESTINATION_HEIGHT, tabH,
            SCREEN_BLIT_COLOR, bgColor,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, fill);

        if (active) {
            int ul[] = {
                SCREEN_BLIT_DESTINATION_X, x,
                SCREEN_BLIT_DESTINATION_Y, tabH - 3,
                SCREEN_BLIT_DESTINATION_WIDTH, tabW,
                SCREEN_BLIT_DESTINATION_HEIGHT, 3,
                SCREEN_BLIT_COLOR, static_cast<int>(0xFF0066CC),
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, ul);
        }

        int tx = x + (tabW - Font::stringWidth(tabLabels[i])) / 2;
        int ty = (tabH - Font::stringHeight()) / 2;
        Font::drawString(ctx, buf, tx, ty, tabLabels[i], textColor);
    }
}

void HistoryScreen::drawBar(screen_context_t ctx, screen_buffer_t buf, int x, int y, int w, int h, float ratio, int color) {
    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, x,
        SCREEN_BLIT_DESTINATION_Y, y,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFDDDDDD),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);
    if (ratio > 0.0f) {
        int fw = (int)(w * ratio);
        if (fw < 1) fw = 1;
        int fb[] = {
            SCREEN_BLIT_DESTINATION_X, x,
            SCREEN_BLIT_DESTINATION_Y, y,
            SCREEN_BLIT_DESTINATION_WIDTH, fw,
            SCREEN_BLIT_DESTINATION_HEIGHT, h,
            SCREEN_BLIT_COLOR, color,
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, fb);
    }
}

void HistoryScreen::drawSessionsTab(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int h = mgr->getHeight();
    int startY = 36;

    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, 0,
        SCREEN_BLIT_DESTINATION_Y, startY,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h - startY,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFFFFFFF),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);

    if (sessions.empty()) {
        std::string msg = mgr->tr("empty_state");
        int tx = (w - Font::stringWidth(msg)) / 2;
        int ty = startY + 20;
        Font::drawString(ctx, buf, tx, ty, msg, 0xFF999999);
        return;
    }

    int y = startY + 4;
    int visible = (h - startY) / 76;

    for (int i = scrollOffset; i < (int)sessions.size() && i < scrollOffset + visible; i++) {
        const auto& s = sessions[i];
        int cardY = y + (i - scrollOffset) * 76;
        if (cardY + 74 > h) break;

        int cardBg[] = {
            SCREEN_BLIT_DESTINATION_X, 4,
            SCREEN_BLIT_DESTINATION_Y, cardY,
            SCREEN_BLIT_DESTINATION_WIDTH, w - 8,
            SCREEN_BLIT_DESTINATION_HEIGHT, 72,
            SCREEN_BLIT_COLOR, static_cast<int>(0xFFF8F8FF),
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, cardBg);

        int corr = s.getCorrectAnswers();
        int wrng = s.getWrongAnswers();
        int total = corr + wrng;
        int pct = (total > 0) ? (corr * 100 / total) : 0;

        char line[64];
        snprintf(line, sizeof(line), "%s (%s: %d, %d%%)",
                 s.getId().substr(0, 8).c_str(),
                 mgr->tr("label_problems").c_str(), total, pct);
        Font::drawString(ctx, buf, 10, cardY + 4, line, 0xFF333333);

        Font::drawString(ctx, buf, 10, cardY + 20,
                         mgr->tr("tab_sessions") + " " + std::to_string(i + 1),
                         0xFF888888);

        if (total > 0) {
            drawBar(ctx, buf, 10, cardY + 38, w - 28, 10,
                    (float)corr / total, 0xFF228B22);
        }

        std::string cStr = mgr->tr("symbol_correct") + " " + std::to_string(corr);
        Font::drawString(ctx, buf, 10, cardY + 52, cStr, 0xFF228B22);
        std::string wStr = mgr->tr("symbol_wrong") + " " + std::to_string(wrng);
        int wx = 10 + Font::stringWidth(cStr) + 20;
        Font::drawString(ctx, buf, wx, cardY + 52, wStr, 0xFFDC143C);
    }
}

void HistoryScreen::drawStatisticsTab(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int h = mgr->getHeight();
    int startY = 36;

    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, 0,
        SCREEN_BLIT_DESTINATION_Y, startY,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h - startY,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFFFFFFF),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);

    if (sessions.empty()) {
        std::string msg = mgr->tr("stats_no_data");
        int tx = (w - Font::stringWidth(msg)) / 2;
        int ty = startY + 20;
        Font::drawString(ctx, buf, tx, ty, msg, 0xFF999999);
        return;
    }

    int totalCorrect = 0, totalWrong = 0;
    for (const auto& s : sessions) {
        totalCorrect += s.getCorrectAnswers();
        totalWrong += s.getWrongAnswers();
    }
    int total = totalCorrect + totalWrong;

    char line[64];
    snprintf(line, sizeof(line), "%s: %d/%d",
             mgr->tr("label_avg_accuracy").c_str(),
             totalCorrect, total);
    Font::drawString(ctx, buf, 10, startY + 8, line, 0xFF333333);

    drawBar(ctx, buf, 10, startY + 30, w - 20, 20,
            total > 0 ? (float)totalCorrect / total : 0, 0xFF228B22);

    std::string symC = mgr->tr("symbol_correct");
    std::string symW = mgr->tr("symbol_wrong");

    snprintf(line, sizeof(line), "%s: %d", symC.c_str(), totalCorrect);
    Font::drawString(ctx, buf, 10, startY + 56, line, 0xFF228B22);

    snprintf(line, sizeof(line), "%s: %d", symW.c_str(), totalWrong);
    Font::drawString(ctx, buf, 10, startY + 72, line, 0xFFDC143C);

    snprintf(line, sizeof(line), "%s: %d", mgr->tr("label_sessions").c_str(), (int)sessions.size());
    Font::drawString(ctx, buf, 10, startY + 88, line, 0xFF555555);
}

void HistoryScreen::drawTablesTab(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int h = mgr->getHeight();
    int startY = 36;

    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, 0,
        SCREEN_BLIT_DESTINATION_Y, startY,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h - startY,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFFFFFFF),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);

    if (sessions.empty()) {
        std::string msg = mgr->tr("stats_no_data");
        int tx = (w - Font::stringWidth(msg)) / 2;
        int ty = startY + 20;
        Font::drawString(ctx, buf, tx, ty, msg, 0xFF999999);
        return;
    }

    int correct[10] = {0};
    int total[10] = {0};

    for (const auto& s : sessions) {
        for (int n = 1; n <= 10; n++) {
            total[n - 1] += s.getMultCorrectForSeries(n) + s.getMultWrongForSeries(n);
            correct[n - 1] += s.getMultCorrectForSeries(n);
        }
    }

    int barW = (w - 20) / 10 - 4;
    if (barW < 12) barW = 12;
    int spacing = 4;
    int maxBarH = h - startY - 60;
    if (maxBarH < 30) maxBarH = 30;
    int startX = (w - (barW + spacing) * 10 + spacing) / 2;

    for (int i = 0; i < 10; i++) {
        int x = startX + i * (barW + spacing);
        float ratio = (total[i] > 0) ? (float)correct[i] / total[i] : 0;
        int barH = (int)(maxBarH * ratio);
        if (barH < 1 && ratio > 0) barH = 1;
        int barY = startY + maxBarH - barH;

        int color = (ratio >= 0.8f) ? 0xFF228B22 :
                    (ratio >= 0.5f) ? 0xFFFFA500 : 0xFFDC143C;

        if (barH > 0) {
            int fb[] = {
                SCREEN_BLIT_DESTINATION_X, x,
                SCREEN_BLIT_DESTINATION_Y, barY,
                SCREEN_BLIT_DESTINATION_WIDTH, barW,
                SCREEN_BLIT_DESTINATION_HEIGHT, barH,
                SCREEN_BLIT_COLOR, color,
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, fb);
        }

        int bgBar[] = {
            SCREEN_BLIT_DESTINATION_X, x,
            SCREEN_BLIT_DESTINATION_Y, startY,
            SCREEN_BLIT_DESTINATION_WIDTH, barW,
            SCREEN_BLIT_DESTINATION_HEIGHT, maxBarH - barH,
            SCREEN_BLIT_COLOR, static_cast<int>(0xFFF0F0F0),
            SCREEN_BLIT_END
        };
        screen_fill(ctx, buf, bgBar);

        char label[16];
        snprintf(label, sizeof(label), mgr->tr("series_label_format").c_str(), i + 1);
        int lx = x + (barW - Font::stringWidth(label)) / 2;
        Font::drawString(ctx, buf, lx, startY + maxBarH + 4, label, 0xFF666666);

        char pct[8];
        snprintf(pct, sizeof(pct), "%d%%", (int)(ratio * 100));
        int px = x + (barW - Font::stringWidth(pct)) / 2;
        Font::drawString(ctx, buf, px, barY - Font::stringHeight() - 2, pct, color);
    }
}

void HistoryScreen::onDraw(screen_context_t ctx, screen_buffer_t buf) {
    drawTabBar(ctx, buf);

    switch (currentTab) {
        case TAB_SESSIONS: drawSessionsTab(ctx, buf); break;
        case TAB_STATISTICS: drawStatisticsTab(ctx, buf); break;
        case TAB_TABLES: drawTablesTab(ctx, buf); break;
    }

    std::string back = mgr->tr("btn_back");
    Font::drawString(ctx, buf, 4, 4, back, 0xFF0066CC);
}

void HistoryScreen::onEvent(int type, int x, int y, int key) {
    int w = mgr->getWidth();

    if (type == 0) {
        if (y < 32) {
            int tabW = w / TAB_COUNT;
            int tab = x / tabW;
            if (tab >= 0 && tab < TAB_COUNT) {
                currentTab = (Tab)tab;
                scrollOffset = 0;
            }
            return;
        }

        if (x < 40 && y < 32) {
            mgr->goBack();
            return;
        }

        if (currentTab == TAB_SESSIONS && y >= 36) {
            int idx = (y - 36) / 76 + scrollOffset;
            (void)idx;
        }
    }
}
