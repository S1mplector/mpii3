/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "playlist.h"
#include "path.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>

int playlist_load(Queue *q, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f)
        return 0;
    memset(q, 0, sizeof(*q));
    char base[MPII3_PATH], line[MPII3_PATH * 2], label[MPII3_TITLE] = "";
    if (!path_copy(base, sizeof(base), path)) {
        fclose(f);
        return 0;
    }
    char *slash = strrchr(base, '/');
    if (slash)
        *slash = 0;
    else
        strcpy(base, ".");
    int first = 1;
    while (fgets(line, sizeof(line), f)) {
        size_t n = strlen(line);
        if (n == sizeof(line) - 1 && line[n - 1] != '\n') {
            int c;
            while ((c = fgetc(f)) != EOF && c != '\n') {
            }
            q->skipped++;
            label[0] = 0;
            continue;
        }
        char *s = line;
        if (first && n >= 3 && (unsigned char)s[0] == 0xef && (unsigned char)s[1] == 0xbb &&
            (unsigned char)s[2] == 0xbf)
            s += 3;
        first = 0;
        while (*s == ' ' || *s == '\t')
            s++;
        n = strlen(s);
        while (n && (s[n - 1] == '\r' || s[n - 1] == '\n' || s[n - 1] == ' ' || s[n - 1] == '\t'))
            s[--n] = 0;
        if (!*s)
            continue;
        if (strncmp(s, "#EXTINF:", 8) == 0) {
            char *comma = strchr(s, ',');
            snprintf(label, sizeof(label), "%s", comma ? comma + 1 : "");
            continue;
        }
        if (*s == '#')
            continue;
        for (char *p = s; *p; ++p)
            if (*p == '\\')
                *p = '/';
        char full[MPII3_PATH];
        struct stat st;
        int absolute = *s == '/' || strncmp(s, "sd:/", 4) == 0 || strncmp(s, "usb:/", 5) == 0;
        int valid =
            absolute ? path_copy(full, sizeof(full), s) : path_join(full, sizeof(full), base, s);
        if (strstr(s, "://") || !valid || !path_has_suffix(s, ".mp3") || stat(full, &st) ||
            !S_ISREG(st.st_mode)) {
            q->skipped++;
            label[0] = 0;
            continue;
        }
        if (q->count == MPII3_MAX_TRACKS) {
            q->truncated = 1;
            break;
        }
        Track *t = &q->tracks[q->count++];
        path_copy(t->path, sizeof(t->path), full);
        track_set_title(t, label);
        label[0] = 0;
    }
    int ok = !ferror(f);
    fclose(f);
    return ok;
}
