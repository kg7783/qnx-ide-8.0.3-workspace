#pragma once

#include <screen/screen.h>

#define KEY_ESC   0xFF1B
#define KEY_DEL   0xFFFF
#define KEY_BSP   0xFF08
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <string>

class Widget;
class ScreenManager;

class Screen {
public:
    Screen() : mgr(nullptr) {}
    explicit Screen(ScreenManager* mgr) : mgr(mgr) {}
    virtual ~Screen() = default;
    virtual void onEnter() {}
    virtual void onLeave() {}
    virtual void onDraw(screen_context_t ctx, screen_buffer_t buf) = 0;
    virtual void onEvent(int type, int x, int y, int key) = 0;
    virtual bool wantsQuit() { return false; }
protected:
    ScreenManager* mgr;
};

class ScreenManager {
public:
    ScreenManager();
    ~ScreenManager();

    bool init(int width, int height);
    void run();
    void quit();

    void addScreen(const std::string& name, std::unique_ptr<Screen> screen);
    void pushScreen(const std::string& name);
    void goBack();

    screen_context_t getCtx() const { return ctx; }
    screen_window_t getWin() const { return win; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    void setStringProvider(const std::function<std::string(const std::string&)>& provider) {
        strProvider = provider;
    }
    std::string tr(const std::string& key) const;

private:
    screen_context_t ctx;
    screen_window_t win;
    screen_buffer_t buffers[2];
    screen_event_t ev;
    int width, height;
    bool running;

    std::vector<std::string> screenStack;
    std::unordered_map<std::string, std::unique_ptr<Screen>> screens;
    Screen* currentScreen = nullptr;
    std::function<std::string(const std::string&)> strProvider;

    void fillRect(screen_buffer_t buf, int x, int y, int w, int h, int color);
    void clear(screen_buffer_t buf, int color);
};
