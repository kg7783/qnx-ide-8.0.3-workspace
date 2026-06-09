#define _POSIX_C_SOURCE 199309L
#include "ScreenManager.h"
#include "Widget.h"
#include "Font.h"
#include <time.h>
#include <cstdio>

ScreenManager::ScreenManager()
    : ctx(nullptr), win(nullptr), width(0), height(0), running(false)
{
    std::fill(buffers, buffers + 2, nullptr);
    ev = nullptr;
}

ScreenManager::~ScreenManager() {
    if (ev) screen_destroy_event(ev);
    if (win) screen_destroy_window(win);
    if (ctx) screen_destroy_context(ctx);
}

bool ScreenManager::init(int w, int h) {
    width = w;
    height = h;

    if (screen_create_context(&ctx, 0) != 0) {
        fprintf(stderr, "Failed to create screen context\n");
        return false;
    }

    if (screen_create_window(&win, ctx) != 0) {
        fprintf(stderr, "Failed to create window\n");
        return false;
    }

    int usage = SCREEN_USAGE_NATIVE | SCREEN_USAGE_WRITE;
    int format = SCREEN_FORMAT_RGBA8888;
    int size[2] = {width, height};
    int pos[2] = {0, 0};
    int bufcount = 1;
    int alphaMode = SCREEN_NON_PRE_MULTIPLIED_ALPHA;
    int visible = 1;

    screen_set_window_property_iv(win, SCREEN_PROPERTY_USAGE, &usage);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_FORMAT, &format);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_SIZE, size);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_POSITION, pos);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_ALPHA_MODE, &alphaMode);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_VISIBLE, &visible);

    if (screen_create_window_buffers(win, bufcount) != 0) {
        fprintf(stderr, "Failed to create window buffers\n");
        return false;
    }

    buffers[0] = nullptr;
    buffers[1] = nullptr;

    screen_get_window_property_pv(win, SCREEN_PROPERTY_RENDER_BUFFERS,
                                  (void**)buffers);

    if (screen_create_event(&ev) != 0) {
        fprintf(stderr, "Failed to create event\n");
        return false;
    }

    fprintf(stderr, "ScreenManager init OK: %dx%d, buf=%p\n", width, height, buffers[0]);
    return true;
}

void ScreenManager::fillRect(screen_buffer_t buf, int x, int y, int w, int h, int color) {
    int a[] = {
        SCREEN_BLIT_DESTINATION_X, x,
        SCREEN_BLIT_DESTINATION_Y, y,
        SCREEN_BLIT_DESTINATION_WIDTH, w,
        SCREEN_BLIT_DESTINATION_HEIGHT, h,
        SCREEN_BLIT_COLOR, color,
        SCREEN_BLIT_END
    };
    screen_fill(ctx, buf, a);
}

void ScreenManager::clear(screen_buffer_t buf, int color) {
    fillRect(buf, 0, 0, width, height, color);
}

void ScreenManager::run() {
    running = true;

    while (running) {
        for (int i = 0; i < 5; i++) {
            if (screen_get_event(ctx, ev, 0) != 0) break;
            int type;
            screen_get_event_property_iv(ev, SCREEN_PROPERTY_TYPE, &type);

            if (type == SCREEN_EVENT_KEYBOARD) {
                int sym;
                screen_get_event_property_iv(ev, SCREEN_PROPERTY_SYM, &sym);

                if (sym == KEY_ESC || sym == 'q' || sym == 'Q') {
                    running = false;
                } else if (currentScreen) {
                    currentScreen->onEvent(type, 0, 0, sym);
                }
            } else if (type == SCREEN_EVENT_MTOUCH_TOUCH) {
                int pos[2];
                screen_get_event_property_iv(ev, SCREEN_PROPERTY_SOURCE_POSITION, pos);
                int px = pos[0], py = pos[1];
                if (currentScreen && px >= 0 && py >= 0) {
                    currentScreen->onEvent(0, px, py, 0);
                }
            } else if (type == SCREEN_EVENT_MTOUCH_RELEASE) {
                int pos[2];
                screen_get_event_property_iv(ev, SCREEN_PROPERTY_SOURCE_POSITION, pos);
                int px = pos[0], py = pos[1];
                if (currentScreen && px >= 0 && py >= 0) {
                    currentScreen->onEvent(1, px, py, 0);
                }
            } else if (type == SCREEN_EVENT_POINTER) {
                static int prevButtons = 0;
                int buttons;
                screen_get_event_property_iv(ev, SCREEN_PROPERTY_BUTTONS, &buttons);
                int pos[2];
                screen_get_event_property_iv(ev, SCREEN_PROPERTY_SOURCE_POSITION, pos);
                int px = pos[0], py = pos[1];
                if (currentScreen && px >= 0 && py >= 0) {
                    if ((buttons & SCREEN_LEFT_MOUSE_BUTTON) && !(prevButtons & SCREEN_LEFT_MOUSE_BUTTON)) {
                        currentScreen->onEvent(0, px, py, 0);
                    } else if (!(buttons & SCREEN_LEFT_MOUSE_BUTTON) && (prevButtons & SCREEN_LEFT_MOUSE_BUTTON)) {
                        currentScreen->onEvent(1, px, py, 0);
                    }
                }
                prevButtons = buttons;
            } else if (type == SCREEN_EVENT_CREATE) {
                screen_buffer_t newBuf;
                screen_get_window_property_pv(win, SCREEN_PROPERTY_RENDER_BUFFERS, (void**)&newBuf);
                buffers[0] = newBuf;
            }
        }

        screen_buffer_t cur;
        screen_get_window_property_pv(win, SCREEN_PROPERTY_RENDER_BUFFERS, (void**)&cur);
        if (!cur) {
            struct timespec ts = {0, 16000000L};
            nanosleep(&ts, NULL);
            continue;
        }

        if (currentScreen) {
            currentScreen->onDraw(ctx, cur);
        } else {
            clear(cur, 0xFF000000);
        }

        screen_flush_blits(ctx, 0);
        screen_post_window(win, cur, 0, NULL, 0);
        struct timespec ts_sleep = {0, 16000000L};
        nanosleep(&ts_sleep, NULL);

        if (currentScreen && currentScreen->wantsQuit()) {
            running = false;
        }
    }
}

void ScreenManager::quit() {
    running = false;
}

void ScreenManager::addScreen(const std::string& name, std::unique_ptr<Screen> screen) {
    screens.insert({name, std::move(screen)});
}

void ScreenManager::pushScreen(const std::string& name) {
    auto it = screens.find(name);
    if (it == screens.end()) return;

    if (currentScreen) currentScreen->onLeave();

    screenStack.push_back(name);
    currentScreen = it->second.get();
    currentScreen->onEnter();
}

void ScreenManager::goBack() {
    if (screenStack.size() <= 1) return;
    if (currentScreen) currentScreen->onLeave();
    screenStack.pop_back();
    const std::string& prev = screenStack.back();
    currentScreen = screens[prev].get();
    currentScreen->onEnter();
}

std::string ScreenManager::tr(const std::string& key) const {
    if (strProvider) return strProvider(key);
    return key;
}
