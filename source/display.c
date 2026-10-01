/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "display.h"
#include "font_render.h"
#include "text_encoding.h"
#include <grrlib.h>
#include <stdlib.h>
extern const unsigned char mpii3_font[], mpii3_font_end[], mpii3_icon[];
static GRRLIB_texImg *logo;
static unsigned font_size(int scale) {
    return scale == 1 ? 12 : (unsigned)scale * 8;
}
void display_init(void) {
    if (GRRLIB_Init() < 0)
        exit(1);
    bool font_ready = font_render_init(mpii3_font, (size_t)(mpii3_font_end - mpii3_font));
    logo = GRRLIB_LoadTexture(mpii3_icon);
    if (!font_ready || !logo) {
        font_render_shutdown();
        GRRLIB_FreeTexture(logo);
        GRRLIB_Exit();
        exit(1);
    }
}
void display_shutdown(void) {
    font_render_shutdown();
    GRRLIB_FreeTexture(logo);
    GRRLIB_Exit();
}
void display_begin(void) {
    GRRLIB_FillScreen(0x0b1020ff);
}
void display_end(void) {
    GRRLIB_Render();
}
void display_logo(int x, int y) {
    GRRLIB_DrawImg(x, y, logo, 0, 1, 1, 0xffffffff);
}
void display_rect(int x, int y, int w, int h, uint32_t rgb) {
    GRRLIB_Rectangle(x, y, w, h, (rgb << 8) | 255, true);
}
void display_text(int x, int y, int scale, uint32_t rgb, const char *value) {
    wchar_t unicode[256];
    text_decode_utf8(value, unicode, 256);
    font_render_draw(x, y, unicode, font_size(scale), (rgb << 8) | 255);
}
void display_text_fit(int x, int y, int scale, uint32_t rgb, const char *value, int columns) {
    if (x >= 612 || columns <= 0)
        return;
    wchar_t unicode[256];
    size_t length = text_decode_utf8(value, unicode, 256);
    unsigned size = font_size(scale), width = (unsigned)(columns * 6 * scale);
    if (width > (unsigned)(612 - x))
        width = (unsigned)(612 - x);
    bool shortened = false;
    while (length && font_render_width(unicode, size) > width) {
        shortened = true;
        unicode[--length] = 0;
    }
    if (shortened && length > 3) {
        unicode[length - 3] = '.';
        unicode[length - 2] = '.';
        unicode[length - 1] = '.';
    }
    font_render_draw(x, y, unicode, size, (rgb << 8) | 255);
}
