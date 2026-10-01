/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_VIEW_H
#define MPII3_VIEW_H
#include "music_types.h"
#include "player.h"
#include "settings.h"
typedef struct {
    const Library *library;
    const Queue *queue;
    const PlayerSnapshot *player;
    size_t selected;
    int current;
    int browsing_playlists;
    const VisualizerSettings *settings;
    int settings_open;
    unsigned settings_row;
    LoopMode loop;
    const char *notice;
} ViewModel;
void view_init(void);
void view_shutdown(void);
void view_loading(const char *message);
void view_render(const ViewModel *model, float elapsed);
#endif
