/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_PATH_H
#define MPII3_PATH_H
#include "music_types.h"
int path_has_suffix(const char *path, const char *extension);
int path_copy(char *destination, size_t capacity, const char *source);
int path_join(char *destination, size_t capacity, const char *base, const char *name);
void track_set_title(Track *track, const char *custom);
#endif
