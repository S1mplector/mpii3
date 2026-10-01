/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_SPECTRUM_H
#define MPII3_SPECTRUM_H
#include <stddef.h>
#define MPII3_BANDS 32
/* Decoder-independent energy mapping, smoothing, and normalization. */
float spectrum_level(float energy);
void spectrum_smooth(float *display, const float *incoming, size_t count, float elapsed);
#endif
