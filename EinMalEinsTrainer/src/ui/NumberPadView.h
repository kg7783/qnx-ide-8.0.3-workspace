#pragma once

#include "Widget.h"
#include <functional>
#include <vector>

class NumberPadView : public Widget {
public:
    NumberPadView() = default;
    NumberPadView(Rect bounds);

    void setOnNumberPressed(std::function<void(int)> cb) { onNumber = cb; }
    void setOnDelete(std::function<void()> cb) { onDelete = cb; }
    void setOnOk(std::function<void()> cb) { onOk = cb; }

    void draw(screen_context_t ctx, screen_buffer_t buf) override;
    bool handleEvent(int type, int x, int y, int key) override;

private:
    static const int COLS = 3;
    static const int ROWS = 4;
    static const int NUM_BUTTONS = 12;

    int buttonValues[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, -1, 0, -2};
    std::string buttonLabels[12] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "DEL", "0", "OK"};

    std::function<void(int)> onNumber;
    std::function<void()> onDelete;
    std::function<void()> onOk;

    int lastPressed = -1;
    int getButtonAt(int x, int y) const;
};
