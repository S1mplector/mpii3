/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_QUEUE_H
#define MPII3_QUEUE_H
#include "music_types.h"
int queue_next(size_t count, int current, int direction, LoopMode loop, int automatic);
#endif
