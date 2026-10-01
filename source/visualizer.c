/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "visualizer.h"
#include "display.h"
#include "spectrum.h"
#include <math.h>
static float levels[MPII3_BANDS];
void visualizer_render(const float *bands, float elapsed, const VisualizerSettings *s) {
    float target[MPII3_BANDS] = {0};
    unsigned width = MPII3_BANDS / s->bars;
    for (unsigned i = 0; i < s->bars; i++) {
        /* Preserve narrow peaks when combining the decoder's 32 subbands. */
        float peak = 0;
        for (unsigned j = 0; j < width; j++)
            if (bands[i * width + j] > peak)
                peak = bands[i * width + j];
        target[i] = fminf(1.0f, peak * s->sensitivity);
    }
    if (s->smoothing < 0.001f) {
        for (unsigned i = 0; i < MPII3_BANDS; i++)
            levels[i] = target[i];
    } else
        spectrum_smooth(levels, target, MPII3_BANDS,
                        elapsed * s->response * (1.65f - s->smoothing));
    if (s->mode == 0) {
        int step = 576 / (int)s->bars, gap = s->bars == 32 ? 6 : 8;
        for (unsigned i = 0; i < s->bars; i++) {
            int height = (int)(levels[i] * 109), x = 32 + (int)i * step;
            rect(x, 319, step - gap, 2, 0x26334b);
            for (int y = 0; y < height; y += 5)
                rect(x, 315 - y, step - gap, 3, y > 65 ? 0xacb9fa : 0x64eaf2);
        }
    } else {
        unsigned points = s->bars * 3;
        for (unsigned i = 0; i < points; i++) {
            float angle = (float)i * 6.2831853f / (float)points;
            float radius = 39 + levels[i / 3] * 30;
            int x = 320 + (int)(cosf(angle) * radius * 2.8f),
                y = 264 + (int)(sinf(angle) * radius * 0.8f);
            rect(x, y, 6, 4, i % 2 ? 0x64eaf2 : 0xacb9fa);
        }
        text(290, 258, 2, 0xeaf1ff, "mpii3");
    }
}
