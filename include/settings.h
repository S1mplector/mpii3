/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_SETTINGS_H
#define MPII3_SETTINGS_H
#define VISUALIZER_SETTING_COUNT 5
typedef struct {
    unsigned bars;
    float sensitivity;
    float response;
    float smoothing;
    int mode;
} VisualizerSettings;
void settings_defaults(VisualizerSettings *settings);
void settings_validate(VisualizerSettings *settings);
void settings_adjust(VisualizerSettings *settings, unsigned row, int direction);
#endif
