/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "spectrum.h"
#include <math.h>
float spectrum_level(float energy) {
    if (!isfinite(energy) || energy <= 0)
        return 0;
    float db = 10.0f * log10f(energy);
    float value = (db + 65.0f) / 65.0f;
    return value < 0 ? 0 : value > 1 ? 1 : value;
}
void spectrum_smooth(float *display, const float *incoming, size_t count, float elapsed) {
    if (elapsed < 0)
        elapsed = 0;
    if (elapsed > 0.25f)
        elapsed = 0.25f;
    for (size_t i = 0; i < count; i++) {
        float target = incoming[i];
        if (!isfinite(target) || target < 0)
            target = 0;
        if (target > 1)
            target = 1;
        float rate = target > display[i] ? 24.0f : 5.0f;
        display[i] += (target - display[i]) * (1.0f - expf(-rate * elapsed));
    }
}
