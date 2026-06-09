#pragma once

#include "Screen.h"
#include "../persistence/SessionManager.h"
#include <vector>

class HistoryScreen : public Screen {
public:
    HistoryScreen(ScreenManager* mgr);
    ~HistoryScreen() override = default;

    void onEnter() override;
    void onLeave() override;
    void onDraw(screen_context_t ctx, screen_buffer_t buf) override;
    void onEvent(int type, int x, int y, int key) override;

private:
    enum Tab { TAB_SESSIONS, TAB_STATISTICS, TAB_TABLES };
    Tab currentTab = TAB_SESSIONS;

    SessionManager sessionMgr;
    std::vector<ProgressSession> sessions;
    int scrollOffset = 0;

    static constexpr int TAB_COUNT = 3;

    void drawTabBar(screen_context_t ctx, screen_buffer_t buf);
    void drawSessionsTab(screen_context_t ctx, screen_buffer_t buf);
    void drawStatisticsTab(screen_context_t ctx, screen_buffer_t buf);
    void drawTablesTab(screen_context_t ctx, screen_buffer_t buf);
    void drawBar(screen_context_t ctx, screen_buffer_t buf, int x, int y, int w, int h, float ratio, int color);
};
