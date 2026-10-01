/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "view.h"
#include "display.h"
#include "visualizer.h"
#include <math.h>
#include <stdio.h>
#define WHITE 0xeaf1ff
#define MUTED 0x8a9bb7
#define MINT 0x58edbb
#define PURPLE 0xac8bfa
static void header(void) {
    display_logo(28, 10);
    display_text(176, 32, 1, MINT, "MUSIC FOR YOUR WII");
    display_rect(28, 61, 584, 2, 0x25334e);
}
void view_init(void) {
    display_init();
}
void view_shutdown(void) {
    display_shutdown();
}
void view_loading(const char *message) {
    display_begin();
    header();
    display_text_fit(28, 140, 2, MUTED, message, 46);
    display_end();
}
void view_render(const ViewModel *m, float elapsed) {
    display_begin();
    header();
    char line[160];
    if (m->settings_open) {
        display_text(28, 75, 1, MINT, "VISUALIZER SETTINGS - LIVE PREVIEW");
        const char *labels[] = {"Style", "Bars", "Sensitivity", "Response speed", "Smoothing"};
        char values[5][32];
        snprintf(values[0], 32, "%s", m->settings->mode ? "Orbit" : "Spectrum");
        snprintf(values[1], 32, "%u", m->settings->bars);
        snprintf(values[2], 32, "%.1fx", m->settings->sensitivity);
        snprintf(values[3], 32, "%.2fx", m->settings->response);
        snprintf(values[4], 32, "%u%%", (unsigned)(m->settings->smoothing * 100 + 0.5f));
        for (unsigned i = 0; i < 5; i++) {
            int y = 94 + (int)i * 21;
            if (i == m->settings_row) {
                display_rect(22, y - 2, 590, 21, 0x1b2b41);
                display_rect(22, y - 2, 3, 21, MINT);
            }
            display_text(34, y, 2, i == m->settings_row ? WHITE : MUTED, labels[i]);
            display_text(440, y, 2, MINT, values[i]);
        }
    } else {
        snprintf(line, sizeof(line), "%s  %u", m->browsing_playlists ? "PLAYLISTS" : "LIBRARY",
                 (unsigned)(m->browsing_playlists ? m->library->playlist_count
                                                  : m->library->music.count));
        display_text(28, 78, 1, MINT, line);
        unsigned total = (unsigned)(m->browsing_playlists ? m->library->playlist_count
                                                          : m->library->music.count);
        snprintf(line, sizeof(line), "%u / %u", total ? (unsigned)m->selected + 1 : 0, total);
        display_text(520, 78, 1, MUTED, line);
        size_t count = m->browsing_playlists ? m->library->playlist_count : m->library->music.count;
        const Track *items =
            m->browsing_playlists ? m->library->playlists : m->library->music.tracks;
        if (!count)
            display_text(28, 109, 2, MUTED,
                         m->browsing_playlists ? "NO M3U PLAYLISTS FOUND"
                                               : "ADD MP3 FILES TO SD:/MUSIC");
        size_t start = m->selected > 1 ? m->selected - 1 : 0;
        for (size_t n = 0; n < 4 && start + n < count; n++) {
            size_t i = start + n;
            int y = 100 + (int)n * 22;
            if (i == m->selected) {
                display_rect(22, y - 4, 590, 22, 0x1b2b41);
                display_rect(22, y - 4, 3, 22, MINT);
            }
            display_text_fit(34, y, 2, i == m->selected ? WHITE : MUTED, items[i].title, 47);
        }
    }
    visualizer_render(m->player->bands, elapsed, m->settings);
    display_rect(28, 337, 584, 1, 0x25334e);
    const char *name = "CHOOSE A TRACK OR PLAYLIST";
    if (m->current >= 0 && (size_t)m->current < m->queue->count)
        name = m->queue->tracks[m->current].title;
    display_text_fit(28, 349, 2, WHITE, name, 48);
    const char *states[] = {"STOPPED", "LOADING", "PLAYING", "PAUSED", "FINISHED", "ERROR"};
    const char *loops[] = {"OFF", "ALL", "ONE"};
    snprintf(line, sizeof(line), "%s  %02u:%02u  VOL %u  LOOP %s", states[m->player->state],
             (unsigned)m->player->seconds / 60, (unsigned)m->player->seconds % 60,
             m->player->volume * 100 / 255, loops[m->loop]);
    display_text(28, 376, 1, MINT, line);
    display_text_fit(28, 393, 1, MUTED, m->notice, 95);
    display_text(28, 422, 1, MUTED,
                 m->settings_open ? "UP/DOWN OPTION   LEFT/RIGHT ADJUST   A RESET DEFAULTS"
                                  : "A PLAY/PAUSE   UP/DOWN SELECT   LEFT/RIGHT SKIP   +/- VOLUME");
    display_text(28, 441, 1, MUTED, "1 LIBRARY/PLAYLISTS   2 LOOP   B SETTINGS/SAVE   HOME EXIT");
    display_end();
}
