#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <screen/screen.h>

#define WIDTH  1920
#define HEIGHT 1200

static int rgba(int r, int g, int b, int a) {
    return (a << 24) | (r << 16) | (g << 8) | b;
}

static void fill_rect(screen_context_t ctx, screen_buffer_t buf, int x, int y, int w, int h, int color) {
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

int main(int argc, char *argv[]) {
    screen_context_t ctx;
    screen_window_t win;
    int size[2] = {WIDTH, HEIGHT};
    int pos[2] = {0, 0};
    int format = SCREEN_FORMAT_RGBA8888;
    int usage = SCREEN_USAGE_NATIVE;
    int bufcount = 2;

    if (screen_create_context(&ctx, 0) != 0) return 1;
    if (screen_create_window(&win, ctx) != 0) return 1;

    screen_set_window_property_iv(win, SCREEN_PROPERTY_USAGE, &usage);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_FORMAT, &format);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_SIZE, size);
    screen_set_window_property_iv(win, SCREEN_PROPERTY_POSITION, pos);
    if (screen_create_window_buffers(win, bufcount) != 0) return 1;

    screen_buffer_t buffers[2];
    screen_get_window_property_pv(win, SCREEN_PROPERTY_RENDER_BUFFERS, (void**)buffers);

    screen_event_t ev;
    screen_create_event(&ev);

    int running = 1, frame = 0;

    while (running) {
        for (int i = 0; i < 10; i++) {
            if (screen_get_event(ctx, ev, 0) != 0) break;
            int type;
            screen_get_event_property_iv(ev, SCREEN_PROPERTY_TYPE, &type);
            if (type == SCREEN_EVENT_KEYBOARD) {
                int sym;
                screen_get_event_property_iv(ev, SCREEN_PROPERTY_SYM, &sym);
                if (sym == 27 || sym == 'q' || sym == 'Q')
                    running = 0;
            }
        }

        screen_buffer_t cur = buffers[frame % 2];
        float t = frame * 0.02f;

        for (int y = 0; y < HEIGHT; y += 120) {
            int r = (int)((sinf(t + y * 0.003f) * 0.5f + 0.5f) * 200 + 55);
            int g = (int)((sinf(t * 0.7f + y * 0.002f) * 0.5f + 0.5f) * 200 + 55);
            int b = (int)((sinf(t * 1.3f + y * 0.001f) * 0.5f + 0.5f) * 200 + 55);
            fill_rect(ctx, cur, 0, y, WIDTH, 120, rgba(r, g, b, 255));
        }

        int cx = WIDTH / 2, cy = HEIGHT / 2;
        int nballs = 8;
        for (int i = 0; i < nballs; i++) {
            float a = t * 0.5f + i * 6.2832f / nballs;
            float r = 200.0f + 80.0f * sinf(t * 0.3f + i * 1.5f);
            int bx = cx + (int)(r * cosf(a));
            int by = cy + (int)(r * 0.75f * sinf(a));
            int bs = 40 + (int)(20.0f * sinf(t * 0.7f + i * 2.0f));
            int ri = (int)((sinf(a + t) * 0.5f + 0.5f) * 255);
            int gi = (int)((sinf(a + t + 2.0f) * 0.5f + 0.5f) * 255);
            int bi = (int)((sinf(a + t + 4.0f) * 0.5f + 0.5f) * 255);
            fill_rect(ctx, cur, bx - bs/2, by - bs/2, bs, bs, rgba(ri, gi, bi, 200));
        }

        int rx = cx + (int)(250.0f * cosf(t * 0.4f));
        int ry = cy + (int)(200.0f * sinf(t * 0.4f));
        int rw = 80 + (int)(40.0f * sinf(t * 0.5f));
        int rh = 80 + (int)(40.0f * cosf(t * 0.6f));
        fill_rect(ctx, cur, rx - rw/2, ry - rh/2, rw, rh, rgba(255, 80, 80, 255));

        int gx = cx + (int)(300.0f * cosf(t * 0.5f + 2.0f));
        int gy = cy + (int)(240.0f * sinf(t * 0.5f + 2.0f));
        fill_rect(ctx, cur, gx - 30, gy - 30, 60, 60, rgba(80, 255, 80, 180));

        int bx = (int)((sinf(t * 0.2f) * 0.5f + 0.5f) * (WIDTH - 100));
        int by = (int)((cosf(t * 0.3f) * 0.5f + 0.5f) * (HEIGHT - 100));
        fill_rect(ctx, cur, bx, by, 100, 100, rgba(100, 100, 255, 220));
        fill_rect(ctx, cur, bx + 20, by + 20, 60, 60, rgba(255, 255, 100, 200));

        for (int i = 0; i < 20; i++) {
            float a = t * 0.5f + i * 6.2832f / 20;
            int x1 = cx + (int)(400.0f * cosf(a));
            int y1 = cy + (int)(300.0f * sinf(a));
            int x2 = cx + (int)(420.0f * cosf(a + 0.1f));
            int y2 = cy + (int)(315.0f * sinf(a + 0.1f));
            int cr = (int)((sinf(a + t) * 0.5f + 0.5f) * 255);
            int cg = (int)((sinf(a + t + 1.5f) * 0.5f + 0.5f) * 255);
            int cb = (int)((sinf(a + t + 3.0f) * 0.5f + 0.5f) * 255);
            for (float p = 0; p <= 1.0f; p += 0.05f) {
                int px = (int)(x1 + (x2 - x1) * p + 0.5f);
                int py = (int)(y1 + (y2 - y1) * p + 0.5f);
                fill_rect(ctx, cur, px, py, 4, 4, rgba(cr, cg, cb, 200));
            }
        }

        screen_post_window(win, cur, 0, NULL, 0);
        usleep(16000);
        frame++;
    }

    screen_destroy_event(ev);
    screen_destroy_window(win);
    screen_destroy_context(ctx);
    return 0;
}
