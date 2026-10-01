/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
bool config_load(const char *path, VisualizerSettings *settings) {
    settings_defaults(settings);
    FILE *f = fopen(path, "r");
    if (!f)
        return false;
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        char key[32], value[64], extra;
        if (sscanf(line, " %31[^=]=%63s %c", key, value, &extra) != 2)
            continue;
        char *end;
        float parsed = strtof(value, &end);
        if (end == value || *end)
            continue;
        if (!strcmp(key, "bars")) {
            if (parsed == 8 || parsed == 16 || parsed == 32)
                settings->bars = (unsigned)parsed;
        } else if (!strcmp(key, "mode")) {
            if (parsed == 0 || parsed == 1)
                settings->mode = (int)parsed;
        } else if (!strcmp(key, "sensitivity"))
            settings->sensitivity = parsed;
        else if (!strcmp(key, "response"))
            settings->response = parsed;
        else if (!strcmp(key, "smoothing"))
            settings->smoothing = parsed;
    }
    bool ok = !ferror(f);
    fclose(f);
    settings_validate(settings);
    return ok;
}
bool config_save(const char *path, const VisualizerSettings *settings) {
    char temp[800];
    int n = snprintf(temp, sizeof(temp), "%s.tmp", path);
    if (n < 0 || (size_t)n >= sizeof(temp))
        return false;
    FILE *f = fopen(temp, "w");
    if (!f)
        return false;
    VisualizerSettings s = *settings;
    settings_validate(&s);
    int wrote =
        fprintf(f,
                "# mpii3 visualizer "
                "settings\nmode=%d\nbars=%u\nsensitivity=%.2f\nresponse=%.2f\nsmoothing=%.2f\n",
                s.mode, s.bars, s.sensitivity, s.response, s.smoothing);
    bool ok = wrote > 0;
    if (fclose(f))
        ok = false;
    if (ok && rename(temp, path) == 0)
        return true;
    remove(temp);
    return false;
}
