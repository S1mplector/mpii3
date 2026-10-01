/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "app.h"
#include "config.h"
#include "discovery.h"
#include "input.h"
#include "player.h"
#include "playlist.h"
#include "queue.h"
#include "view.h"
#include <fat.h>
#include <gccore.h>
#include <ogc/lwp_watchdog.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
/* Application owns navigation/queue state. Platform services stay in modules. */
static Library library;
static Queue queue, candidate;
static int current = -1, playlist_view, consecutive_failures;
static size_t selected;
static VisualizerSettings settings;
static int settings_open, settings_dirty;
static unsigned settings_row;
static const char *settings_path = "sd:/apps/mpii3/settings.ini";
static char active_playlist[MPII3_PATH];
static LoopMode loop = LOOP_ALL;
static char notice[128] = "Ready. Select music with the Wii Remote.";
static void save_settings(void) {
    if (!settings_dirty)
        return;
    if (config_save(settings_path, &settings)) {
        settings_dirty = 0;
        snprintf(notice, sizeof(notice), "Visualizer settings saved.");
    } else
        snprintf(notice, sizeof(notice), "Settings apply now; could not save to the app folder.");
}
static void start(int index) {
    current = index;
    if (index >= 0 && (size_t)index < queue.count)
        player_play(queue.tracks[index].path);
    else {
        current = -1;
        player_stop();
    }
}
static void advance(int direction, int automatic, int error) {
    LoopMode policy = error && loop == LOOP_ONE ? LOOP_ALL : loop;
    start(queue_next(queue.count, current, direction, policy, automatic));
}
static void activate(void) {
    if (playlist_view) {
        if (selected >= library.playlist_count)
            return;
        if (current >= 0 && !strcmp(active_playlist, library.playlists[selected].path)) {
            PlayerSnapshot status;
            player_snapshot(&status);
            if (status.state == PLAYER_PLAYING || status.state == PLAYER_PAUSED) {
                player_toggle_pause();
                return;
            }
        }
        if (!playlist_load(&candidate, library.playlists[selected].path) || !candidate.count) {
            snprintf(notice, sizeof(notice), "Playlist has no readable local MP3 files.");
            return;
        }
        player_stop();
        queue = candidate;
        snprintf(active_playlist, sizeof(active_playlist), "%s", library.playlists[selected].path);
        consecutive_failures = 0;
        start(0);
        snprintf(notice, sizeof(notice), "Playlist loaded: %u tracks, %u skipped%s.",
                 (unsigned)queue.count, queue.skipped, queue.truncated ? ", LIMIT REACHED" : "");
    } else {
        if (selected >= library.music.count)
            return;
        if (current >= 0 && (size_t)current < queue.count &&
            !strcmp(queue.tracks[current].path, library.music.tracks[selected].path)) {
            PlayerSnapshot s;
            player_snapshot(&s);
            if (s.state == PLAYER_PLAYING || s.state == PLAYER_PAUSED ||
                s.state == PLAYER_STARTING) {
                player_toggle_pause();
                return;
            }
        }
        player_stop();
        queue = library.music;
        active_playlist[0] = 0;
        consecutive_failures = 0;
        start((int)selected);
        snprintf(notice, sizeof(notice), "Playing the music library.");
    }
}
int app_run(void) {
    view_init();
    input_init();
    player_init();
    view_loading("Discovering music on SD and USB...");
    if (fatInitDefault()) {
        library_scan(&library, "sd:/Music");
        library_scan(&library, "usb:/Music");
        snprintf(notice, sizeof(notice), "Found %u tracks and %u playlists%s.",
                 (unsigned)library.music.count, (unsigned)library.playlist_count,
                 library.music.truncated ? " - library limit reached" : "");
    } else
        snprintf(notice, sizeof(notice), "No storage mounted. Insert SD/USB and relaunch.");
    if (access("sd:/apps/mpii3", F_OK) != 0 && access("usb:/apps/mpii3", F_OK) == 0)
        settings_path = "usb:/apps/mpii3/settings.ini";
    config_load(settings_path, &settings);
    u64 last = gettime();
    while (SYS_MainLoop()) {
        Input in = input_read();
        if (in.exit)
            break;
        PlayerSnapshot s;
        player_snapshot(&s);
        if (current >= 0 && (s.state == PLAYER_FINISHED || s.state == PLAYER_ERROR)) {
            int error = s.state == PLAYER_ERROR;
            if (error) {
                consecutive_failures++;
                snprintf(notice, sizeof(notice), "Skipped unreadable track: %.85s", s.error);
            } else
                consecutive_failures = 0;
            if (error && (size_t)consecutive_failures >= queue.count) {
                start(-1);
                snprintf(notice, sizeof(notice),
                         "No playable tracks in this queue. Choose another playlist.");
            } else
                advance(1, 1, error);
        }
        if (in.visualizer) {
            settings_open = !settings_open;
            if (!settings_open)
                save_settings();
        }
        if (settings_open) {
            int row = (int)settings_row + in.selection;
            if (row < 0)
                row = 0;
            if (row >= VISUALIZER_SETTING_COUNT)
                row = VISUALIZER_SETTING_COUNT - 1;
            settings_row = (unsigned)row;
            if (in.skip) {
                settings_adjust(&settings, settings_row, in.skip);
                settings_dirty = 1;
            }
            if (in.activate) {
                settings_defaults(&settings);
                settings_dirty = 1;
            }
        }
        if (!settings_open && in.library) {
            playlist_view = !playlist_view;
            selected = 0;
        }
        size_t count = playlist_view ? library.playlist_count : library.music.count;
        if (!settings_open && count && in.selection) {
            int n = (int)selected + in.selection;
            if (n < 0)
                n = 0;
            if ((size_t)n >= count)
                n = (int)count - 1;
            selected = (size_t)n;
        }
        if (in.loop)
            loop = (LoopMode)((loop + 1) % 3);
        if (in.volume)
            player_set_volume((int)s.volume + in.volume);
        if (!settings_open && in.skip && current >= 0) {
            consecutive_failures = 0;
            advance(in.skip, 0, 0);
        }
        if (!settings_open && in.activate)
            activate();
        player_snapshot(&s);
        u64 now = gettime();
        float elapsed = (float)ticks_to_microsecs(now - last) / 1000000.0f;
        last = now;
        ViewModel model = {.library = &library,
                           .queue = &queue,
                           .player = &s,
                           .selected = selected,
                           .current = current,
                           .browsing_playlists = playlist_view,
                           .settings = &settings,
                           .settings_open = settings_open,
                           .settings_row = settings_row,
                           .loop = loop,
                           .notice = notice};
        view_render(&model, elapsed);
    }
    save_settings();
    player_shutdown();
    view_shutdown();
    return 0;
}
