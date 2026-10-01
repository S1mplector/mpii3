/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_VISUALIZER_H
#define MPII3_VISUALIZER_H
#include "settings.h"
void visualizer_render(const float *bands, float elapsed, const VisualizerSettings *settings);
#endif
