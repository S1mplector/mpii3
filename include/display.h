/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_DISPLAY_H
#define MPII3_DISPLAY_H
#include <stdint.h>
void display_init(void);
void display_logo(int x, int y);
void display_shutdown(void);
void display_begin(void);
void display_end(void);
void rect(int x, int y, int w, int h, uint32_t rgb);
void text(int x, int y, int scale, uint32_t rgb, const char *value);
void text_fit(int x, int y, int scale, uint32_t rgb, const char *value, int columns);
#endif
