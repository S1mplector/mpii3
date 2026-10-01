/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "discovery.h"
#include "path.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

static int compare(const void *a, const void *b) {
    const Track *x = a, *y = b;
    int c = strcasecmp(x->title, y->title);
    return c ? c : strcmp(x->path, y->path);
}
static int scan(Library *lib, const char *root, unsigned depth) {
    DIR *dir = opendir(root);
    if (!dir)
        return 0;
    struct dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_name[0] == '.')
            continue;
        char path[MPII3_PATH];
        struct stat st;
        if (!path_join(path, sizeof(path), root, entry->d_name) || lstat(path, &st)) {
            lib->music.skipped++;
            continue;
        }
        if (S_ISDIR(st.st_mode)) {
            if (depth < 16) {
                if (!scan(lib, path, depth + 1))
                    lib->music.skipped++;
            } else
                lib->music.skipped++;
        } else if (S_ISREG(st.st_mode) && path_has_suffix(path, ".mp3")) {
            if (lib->music.count == MPII3_MAX_TRACKS) {
                lib->music.truncated = 1;
                continue;
            }
            Track *t = &lib->music.tracks[lib->music.count++];
            path_copy(t->path, sizeof(t->path), path);
            track_set_title(t, NULL);
        } else if (S_ISREG(st.st_mode) &&
                   (path_has_suffix(path, ".m3u") || path_has_suffix(path, ".m3u8"))) {
            if (lib->playlist_count == MPII3_MAX_PLAYLISTS) {
                lib->music.truncated = 1;
                continue;
            }
            Track *t = &lib->playlists[lib->playlist_count++];
            path_copy(t->path, sizeof(t->path), path);
            track_set_title(t, NULL);
        }
    }
    closedir(dir);
    return 1;
}
int library_scan(Library *lib, const char *root) {
    int ok = scan(lib, root, 0);
    qsort(lib->music.tracks, lib->music.count, sizeof(Track), compare);
    qsort(lib->playlists, lib->playlist_count, sizeof(Track), compare);
    return ok;
}
