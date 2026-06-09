#pragma once

#include "Widget.h"
#include "Font.h"
#include <functional>
#include <string>

class Button : public Widget {
public:
    Button() = default;
    Button(Rect bounds, const std::string& text, std::function<void()> onClick = nullptr);

    void setText(const std::string& text) { label = text; }
    const std::string& getText() const { return label; }

    void setOnClick(std::function<void()> cb) { onClick = cb; }
    void setColors(Color bg, Color text, Color border);
    void setPressedColor(Color c) { pressedBgColor = c; }

    void draw(screen_context_t ctx, screen_buffer_t buf) override;
    bool handleEvent(int type, int x, int y, int key) override;

private:
    std::string label;
    std::function<void()> onClick;
    Color bgColor{220, 220, 255, 255};
    Color textColor{0, 0, 0, 255};
    Color borderColor{200, 200, 200, 255};
    Color pressedBgColor{180, 180, 240, 255};
    bool pressed = false;
};
