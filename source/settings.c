/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "settings.h"
#include <math.h>
static float clamp(float value, float low, float high, float fallback) {
    if (!isfinite(value))
        return fallback;
    return value < low ? low : value > high ? high : value;
}
void settings_defaults(VisualizerSettings *s) {
    *s = (VisualizerSettings){32, 1.0f, 1.0f, 0.65f, 0};
}
void settings_validate(VisualizerSettings *s) {
    if (s->bars != 8 && s->bars != 16 && s->bars != 32)
        s->bars = 32;
    s->sensitivity = clamp(s->sensitivity, 0.5f, 3.0f, 1.0f);
    s->response = clamp(s->response, 0.25f, 3.0f, 1.0f);
    s->smoothing = clamp(s->smoothing, 0, 0.95f, 0.65f);
    if (s->mode != 0 && s->mode != 1)
        s->mode = 0;
}
void settings_adjust(VisualizerSettings *s, unsigned row, int direction) {
    if (!direction)
        return;
    int d = direction < 0 ? -1 : 1;
    switch (row) {
    case 0:
        s->mode = !s->mode;
        break;
    case 1:
        if (d > 0 && s->bars < 32)
            s->bars *= 2;
        else if (d < 0 && s->bars > 8)
            s->bars /= 2;
        break;
    case 2:
        s->sensitivity += d * 0.1f;
        break;
    case 3:
        s->response += d * 0.25f;
        break;
    case 4:
        s->smoothing += d * 0.05f;
        break;
    }
    settings_validate(s);
}
