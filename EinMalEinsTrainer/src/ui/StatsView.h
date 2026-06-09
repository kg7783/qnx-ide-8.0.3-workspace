#pragma once

#include "Widget.h"

class StatsView : public Widget {
public:
    StatsView() = default;
    StatsView(Rect bounds);

    void setStats(int correct, int wrong);
    void draw(screen_context_t ctx, screen_buffer_t buf) override;
    bool handleEvent(int type, int x, int y, int key) override { return false; }

private:
    int correct = 0;
    int wrong = 0;
    int total = 0;
};
