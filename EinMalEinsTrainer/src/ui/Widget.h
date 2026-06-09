#pragma once

#include <screen/screen.h>
#include <functional>
#include <string>
#include <vector>
#include <cstdint>

struct Color {
    uint8_t r, g, b, a;
    Color() : r(0), g(0), b(0), a(255) {}
    Color(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}
    int toARGB() const { return (a << 24) | (r << 16) | (g << 8) | b; }
};

struct Rect {
    int x, y, w, h;
    Rect() : x(0), y(0), w(0), h(0) {}
    Rect(int x_, int y_, int w_, int h_) : x(x_), y(y_), w(w_), h(h_) {}
    bool contains(int px, int py) const {
        return px >= x && px < x + w && py >= y && py < y + h;
    }
};

class Widget {
public:
    Widget() = default;
    Widget(Rect bounds) : bounds(bounds) {}
    virtual ~Widget() = default;

    void setBounds(Rect r) { bounds = r; }
    Rect getBounds() const { return bounds; }
    int getX() const { return bounds.x; }
    int getY() const { return bounds.y; }
    int getWidth() const { return bounds.w; }
    int getHeight() const { return bounds.h; }

    virtual void draw(screen_context_t ctx, screen_buffer_t buf) = 0;
    virtual bool handleEvent(int type, int x, int y, int key) { return false; }

    bool isVisible() const { return visible; }
    void setVisible(bool v) { visible = v; }
    bool isEnabled() const { return enabled; }
    void setEnabled(bool e) { enabled = e; }

public:
    Rect bounds;
    bool visible = true;
    bool enabled = true;
};
