#pragma once

#include "Screen.h"
#include "Widget.h"
#include "../persistence/ConfigManager.h"
#include <array>

class SetupScreen : public Screen {
public:
    SetupScreen(ScreenManager* mgr);
    ~SetupScreen() override = default;

    void onEnter() override;
    void onLeave() override;
    void onDraw(screen_context_t ctx, screen_buffer_t buf) override;
    void onEvent(int type, int x, int y, int key) override;

private:
    static const int GRID = 10;
    bool multipliers[GRID][GRID];

    int langSel = 0;
    bool autoSwitch = true;

    std::array<Rect, GRID * GRID> gridRects;

    ConfigManager configMgr;

    void drawGrid(screen_context_t ctx, screen_buffer_t buf);
    void drawBottomBar(screen_context_t ctx, screen_buffer_t buf);
    int getGridCell(int px, int py) const;
    void saveConfig();
    void toggleCell(int idx);
};
