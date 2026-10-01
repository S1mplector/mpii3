/* SPDX-License-Identifier: GPL-2.0-or-later */
#define _POSIX_C_SOURCE 200809L
#include "config.h"
#include "discovery.h"
#include "playlist.h"
#include "queue.h"
#include "spectrum.h"
#include "text_encoding.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
static Library library;
static Queue queue;
static void put(const char *path, const char *contents) {
    FILE *f = fopen(path, "wb");
    assert(f);
    fputs(contents, f);
    assert(fclose(f) == 0);
}
static void test_discovery_and_playlists(void) {
    char root[] = "/tmp/mpii3-test-XXXXXX";
    assert(mkdtemp(root));
    assert(chdir(root) == 0);
    assert(mkdir("nested", 0700) == 0);
    put("zeta.MP3", "");
    put("nested/alpha.mp3", "");
    put("ignore.wav", "");
    assert(symlink(root, "nested/cycle") == 0);
    put("mix.m3u", "\xef\xbb\xbf#EXTM3U\r\n#EXTINF:12,Custom "
                   "title\r\nnested\\alpha.mp3\r\nzeta.MP3\nmissing.mp3\nhttps://example.com/"
                   "music.mp3\nignore.wav\n");
    assert(library_scan(&library, root));
    assert(library.music.count == 2);
    assert(library.playlist_count == 1);
    assert(!strcmp(library.music.tracks[0].title, "alpha"));
    assert(playlist_load(&queue, "mix.m3u"));
    assert(queue.count == 2);
    assert(queue.skipped == 3);
    assert(!strcmp(queue.tracks[0].title, "Custom title"));
    assert(!strcmp(queue.tracks[1].title, "zeta"));
    put("nested/relative.m3u", "../zeta.MP3\n../nested/alpha.mp3\n");
    assert(playlist_load(&queue, "nested/relative.m3u"));
    assert(queue.count == 2);
    put("empty.m3u", "#EXTM3U\nmissing.mp3\n");
    assert(playlist_load(&queue, "empty.m3u"));
    assert(!queue.count);
    assert(!playlist_load(&queue, "absent.m3u"));
    FILE *f = fopen("oversize.m3u", "wb");
    assert(f);
    for (int i = 0; i < 4000; i++)
        fputc('x', f);
    fputs("\nzeta.MP3\n", f);
    fclose(f);
    assert(playlist_load(&queue, "oversize.m3u"));
    assert(queue.count == 1 && queue.skipped == 1);
    f = fopen("limit.m3u", "wb");
    assert(f);
    for (int i = 0; i < MPII3_MAX_TRACKS + 5; i++)
        fputs("zeta.MP3\n", f);
    fclose(f);
    assert(playlist_load(&queue, "limit.m3u"));
    assert(queue.count == MPII3_MAX_TRACKS && queue.truncated);
    unlink("nested/cycle");
    unlink("nested/alpha.mp3");
    unlink("nested/relative.m3u");
    rmdir("nested");
    unlink("zeta.MP3");
    unlink("ignore.wav");
    unlink("mix.m3u");
    unlink("empty.m3u");
    unlink("oversize.m3u");
    unlink("limit.m3u");
    assert(chdir("/") == 0);
    assert(rmdir(root) == 0);
}
static void test_queue(void) {
    assert(queue_next(0, 0, 1, LOOP_ALL, 1) == -1);
    assert(queue_next(3, -1, 1, LOOP_ALL, 1) == -1);
    assert(queue_next(3, 0, 1, LOOP_OFF, 1) == 1);
    assert(queue_next(3, 2, 1, LOOP_OFF, 1) == -1);
    assert(queue_next(3, 2, 1, LOOP_ALL, 1) == 0);
    assert(queue_next(3, 0, -1, LOOP_ALL, 0) == 2);
    assert(queue_next(3, 1, 1, LOOP_ONE, 1) == 1);
    assert(queue_next(3, 1, 1, LOOP_ONE, 0) == 2);
    assert(queue_next(1, 0, 1, LOOP_ALL, 1) == 0);
}
static void test_spectrum(void) {
    assert(spectrum_level(0) == 0);
    assert(spectrum_level(NAN) == 0);
    assert(spectrum_level(1) == 1);
    assert(spectrum_level(0.1f) > spectrum_level(0.0001f));
    float levels[2] = {0, 0}, incoming[2] = {1, NAN};
    spectrum_smooth(levels, incoming, 2, 1.0f / 60);
    assert(levels[0] > 0 && levels[0] < 1);
    assert(levels[1] == 0);
    float old = levels[0];
    incoming[0] = 0;
    spectrum_smooth(levels, incoming, 2, 1.0f / 60);
    assert(levels[0] < old && levels[0] > 0);
}
static void test_settings(void) {
    VisualizerSettings s;
    settings_defaults(&s);
    assert(s.bars == 32 && s.mode == 0);
    settings_adjust(&s, 1, -1);
    assert(s.bars == 16);
    settings_adjust(&s, 1, -1);
    settings_adjust(&s, 1, -1);
    assert(s.bars == 8);
    for (int i = 0; i < 100; i++)
        settings_adjust(&s, 4, 1);
    assert(s.smoothing <= 0.95f);
    s.sensitivity = NAN;
    s.response = INFINITY;
    s.bars = 999;
    s.mode = -1;
    settings_validate(&s);
    assert(s.sensitivity == 1 && s.response == 1 && s.bars == 32 && s.mode == 0);
    char path[] = "/tmp/mpii3-settings-XXXXXX";
    int fd = mkstemp(path);
    assert(fd >= 0);
    close(fd);
    s.bars = 16;
    s.sensitivity = 1.7f;
    s.response = 2.0f;
    s.mode = 1;
    assert(config_save(path, &s));
    VisualizerSettings loaded;
    assert(config_load(path, &loaded));
    assert(loaded.bars == 16 && loaded.mode == 1 && fabsf(loaded.sensitivity - 1.7f) < 0.001f);
    assert(config_save(path, &s)); /* Existing file replacement. */
    put(path, "bars=0\nmode=99999999999999999999\nsensitivity=nan\nresponse=inf\nsmoothing=-"
              "9\nunknown=1\n");
    assert(config_load(path, &loaded));
    assert(loaded.bars == 32 && loaded.mode == 0 && loaded.sensitivity == 1 &&
           loaded.response == 1 && loaded.smoothing == 0);
    unlink(path);
    assert(!config_load(path, &loaded));
    assert(loaded.bars == 32);
    assert(!config_save("/nonexistent-mpii3-folder/config.ini", &s));
}
int main(void) {
    test_discovery_and_playlists();
    test_queue();
    test_settings();
    test_spectrum();
    wchar_t unicode[20];
    assert(text_decode_utf8("caf\xc3\xa9", unicode, 20) == 4 && unicode[3] == 0xe9);
    assert(text_decode_utf8("\xff"
                            "A",
                            unicode, 20) == 2 &&
           unicode[0] == 0xfffd && unicode[1] == 'A');
    assert(text_decode_utf8("\xe2\x82", unicode, 20) == 2);
    assert(text_decode_utf8("\xf0\x9f\x8e\xb5", unicode, 20) == 1 && unicode[0] == 0x1f3b5);
    assert(text_decode_utf8("abc", unicode, 2) == 1 && unicode[1] == 0);
    puts("PASS: discovery, playlist edge cases, queue/loop transitions, spectrum, settings "
         "persistence, UTF-8");
    return 0;
}
