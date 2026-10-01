/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef MPII3_PLAYER_H
#define MPII3_PLAYER_H
#include <stdbool.h>
#define SPECTRUM_BANDS 32
typedef enum {
    PLAYER_STOPPED,
    PLAYER_STARTING,
    PLAYER_PLAYING,
    PLAYER_PAUSED,
    PLAYER_FINISHED,
    PLAYER_ERROR
} PlayerState;
typedef struct {
    PlayerState state;
    unsigned volume;
    unsigned decoded_frames;
    float seconds;
    float bands[SPECTRUM_BANDS];
    char error[96];
} PlayerSnapshot;
void player_init(void);
bool player_play(const char *path);
void player_stop(void);
void player_toggle_pause(void);
void player_set_volume(int volume);
void player_snapshot(PlayerSnapshot *snapshot);
void player_shutdown(void);
#endif
