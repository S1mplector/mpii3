/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_CONFIG_H
#define MPII3_CONFIG_H
#include "settings.h"
#include <stdbool.h>
bool config_load(const char *path, VisualizerSettings *settings);
bool config_save(const char *path, const VisualizerSettings *settings);
#endif
