/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "path.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
int path_has_suffix(const char *path, const char *ext) {
    size_t n = strlen(path), e = strlen(ext);
    return n >= e && strcasecmp(path + n - e, ext) == 0;
}
int path_copy(char *dst, size_t cap, const char *src) {
    if (strlen(src) >= cap)
        return 0;
    strcpy(dst, src);
    return 1;
}
int path_join(char *dst, size_t cap, const char *base, const char *name) {
    int n = snprintf(dst, cap, "%s/%s", base, name);
    return n >= 0 && (size_t)n < cap;
}
void track_set_title(Track *t, const char *custom) {
    const char *name = custom && *custom ? custom : strrchr(t->path, '/');
    if (!(custom && *custom))
        name = name ? name + 1 : t->path;
    snprintf(t->title, sizeof(t->title), "%.127s", name);
    if (!(custom && *custom)) {
        char *dot = strrchr(t->title, '.');
        if (dot)
            *dot = 0;
    }
    for (char *p = t->title; *p; ++p)
        if ((unsigned char)*p < 32)
            *p = ' ';
}
