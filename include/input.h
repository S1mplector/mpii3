/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_INPUT_H
#define MPII3_INPUT_H
#include <stdbool.h>
typedef struct {
    int selection, skip, volume;
    bool activate, library, loop, visualizer, exit;
} Input;
void input_init(void);
Input input_read(void);
#endif
