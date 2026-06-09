#include "SetupScreen.h"
#include "Font.h"
#include <cstdio>

SetupScreen::SetupScreen(ScreenManager* mgr)
    : Screen(mgr), configMgr()
{
    std::fill(&multipliers[0][0], &multipliers[0][0] + GRID * GRID, false);
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            multipliers[i][j] = true;
            gridRects[i * GRID + j] = Rect{0, 0, 0, 0};
        }
    }
}

void SetupScreen::onEnter() {
    int w = mgr->getWidth();
    int h = mgr->getHeight();

    int gridW = (w > h) ? h - 60 : w - 20;
    int gridH = gridW;
    if (gridW > w - 40) gridW = w - 40;
    if (gridH > h - 80) gridH = h - 80;

    int gridX = (w - gridW) / 2;
    int gridY = 10;
    int cellW = gridW / GRID;
    int cellH = gridH / GRID;

    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            gridRects[i * GRID + j] = Rect{
                gridX + j * cellW,
                gridY + i * cellH,
                cellW,
                cellH
            };
        }
    }

    auto cfg = configMgr.loadConfig();
    for (int i = 1; i <= 10; i++) {
        for (int j = 1; j <= 10; j++) {
            multipliers[i-1][j-1] = cfg.multipliers[i].count(j) > 0;
        }
    }
    langSel = (cfg.language == "de") ? 1 : 0;
    autoSwitch = cfg.autoSwitchMode;
}

void SetupScreen::onLeave() {
    saveConfig();
}

int SetupScreen::getGridCell(int px, int py) const {
    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            if (gridRects[i * GRID + j].contains(px, py)) {
                return i * GRID + j;
            }
        }
    }
    return -1;
}

void SetupScreen::toggleCell(int idx) {
    if (idx < 0 || idx >= GRID * GRID) return;
    int i = idx / GRID;
    int j = idx % GRID;
    multipliers[i][j] = !multipliers[i][j];

    bool any = false;
    for (int a = 0; a < GRID && !any; a++) {
        for (int b = 0; b < GRID && !any; b++) {
            if (multipliers[a][b]) any = true;
        }
    }
    if (!any) multipliers[i][j] = true;
}

void SetupScreen::saveConfig() {
    EinmaleinsConfig cfg;
    for (int i = 1; i <= 10; i++) {
        cfg.multipliers[i].clear();
        for (int j = 1; j <= 10; j++) {
            if (multipliers[i-1][j-1]) {
                cfg.multipliers[i].insert(j);
                cfg.baseNumbers.insert(j);
            }
        }
    }
    cfg.language = (langSel == 0) ? "en" : "de";
    cfg.autoSwitchMode = autoSwitch;
    configMgr.saveConfig(cfg);
}

void SetupScreen::drawGrid(screen_context_t ctx, screen_buffer_t buf) {
    int onColor = 0xFF3A7BD5;
    int offColor = 0xFFE0E0E0;
    int onTextColor = 0xFFFFFFFF;
    int offTextColor = 0xFF999999;
    int borderColor = 0xFFBBBBBB;

    for (int i = 0; i < GRID; i++) {
        for (int j = 0; j < GRID; j++) {
            const Rect& r = gridRects[i * GRID + j];
            if (r.w <= 0) continue;

            bool on = multipliers[i][j];

            int fill[] = {
                SCREEN_BLIT_DESTINATION_X, r.x,
                SCREEN_BLIT_DESTINATION_Y, r.y,
                SCREEN_BLIT_DESTINATION_WIDTH, r.w,
                SCREEN_BLIT_DESTINATION_HEIGHT, r.h,
                SCREEN_BLIT_COLOR, on ? onColor : offColor,
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, fill);

            char label[4];
            snprintf(label, sizeof(label), "%dx%d", i + 1, j + 1);

            if (r.w >= Font::stringWidth(label) + 4) {
                int tx = r.x + (r.w - Font::stringWidth(label)) / 2;
                int ty = r.y + (r.h - Font::stringHeight()) / 2;
                Font::drawString(ctx, buf, tx, ty, label,
                                 on ? onTextColor : offTextColor);
            }

            int bdr[] = {
                SCREEN_BLIT_DESTINATION_X, r.x,
                SCREEN_BLIT_DESTINATION_Y, r.y,
                SCREEN_BLIT_DESTINATION_WIDTH, r.w,
                SCREEN_BLIT_DESTINATION_HEIGHT, 1,
                SCREEN_BLIT_COLOR, borderColor,
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, bdr);
            bdr[3] = r.y + r.h - 1;
            screen_fill(ctx, buf, bdr);

            int bdr2[] = {
                SCREEN_BLIT_DESTINATION_X, r.x,
                SCREEN_BLIT_DESTINATION_Y, r.y,
                SCREEN_BLIT_DESTINATION_WIDTH, 1,
                SCREEN_BLIT_DESTINATION_HEIGHT, r.h,
                SCREEN_BLIT_COLOR, borderColor,
                SCREEN_BLIT_END
            };
            screen_fill(ctx, buf, bdr2);
            bdr2[1] = r.x + r.w - 1;
            screen_fill(ctx, buf, bdr2);
        }
    }
}

void SetupScreen::drawBottomBar(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int h = mgr->getHeight();
    int y = h - 40;

    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, 0,
        SCREEN_BLIT_DESTINATION_Y, y,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, 40,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFF5F5F5),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);

    std::string langLabel = mgr->tr("label_language") + ": ";
    langLabel += (langSel == 0) ? "EN" : "DE";
    Font::drawString(ctx, buf, 8, y + 4, langLabel, 0xFF333333);

    std::string autoLabel = mgr->tr("label_auto_switch") + ": ";
    autoLabel += autoSwitch ? mgr->tr("btn_yes") : mgr->tr("btn_no");
    Font::drawString(ctx, buf, 8, y + 20, autoLabel, 0xFF333333);

    std::string backLabel = mgr->tr("btn_back");
    int bx = w - Font::stringWidth(backLabel) - 12;
    Font::drawString(ctx, buf, bx, y + 12, backLabel, 0xFF0066CC);
}

void SetupScreen::onDraw(screen_context_t ctx, screen_buffer_t buf) {
    int w = mgr->getWidth();
    int h = mgr->getHeight();

    int bg[] = {
        SCREEN_BLIT_DESTINATION_X, 0,
        SCREEN_BLIT_DESTINATION_Y, 0,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, static_cast<int>(0xFFFFFFFF),
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, bg);

    std::string title = mgr->tr("title_setup");
    int cx = (w - Font::stringWidth(title)) / 2;
    Font::drawString(ctx, buf, cx, 2, title, 0xFF333333);

    drawGrid(ctx, buf);
    drawBottomBar(ctx, buf);
}

void SetupScreen::onEvent(int type, int x, int y, int key) {
    if (type == 0) {
        int idx = getGridCell(x, y);
        if (idx >= 0) {
            toggleCell(idx);
            saveConfig();
            return;
        }

        int h = mgr->getHeight();
        if (y >= h - 40) {
            int w = mgr->getWidth();

            if (x < w / 3) {
                autoSwitch = !autoSwitch;
                saveConfig();
            } else if (x < w * 2 / 3) {
                langSel = (langSel + 1) % 2;
                saveConfig();
            } else {
                mgr->goBack();
            }
        }
    }
}
