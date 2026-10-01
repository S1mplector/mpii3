/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_MUSIC_TYPES_H
#define MPII3_MUSIC_TYPES_H
#include <stddef.h>
#define MPII3_MAX_TRACKS 2048
#define MPII3_MAX_PLAYLISTS 256
#define MPII3_PATH 768
#define MPII3_TITLE 128
typedef struct {
    char path[MPII3_PATH];
    char title[MPII3_TITLE];
} Track;
typedef struct {
    Track tracks[MPII3_MAX_TRACKS];
    size_t count;
    unsigned skipped;
    int truncated;
} Queue;
typedef struct {
    Queue music;
    Track playlists[MPII3_MAX_PLAYLISTS];
    size_t playlist_count;
} Library;
typedef enum { LOOP_OFF, LOOP_ALL, LOOP_ONE } LoopMode;
#endif
