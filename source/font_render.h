/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_FONT_RENDER_H
#define MPII3_FONT_RENDER_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>
bool font_render_init(const unsigned char *data, size_t bytes);
void font_render_shutdown(void);
unsigned font_render_width(const wchar_t *text, unsigned size);
void font_render_draw(int x, int y, const wchar_t *text, unsigned size, uint32_t rgba);
#endif
