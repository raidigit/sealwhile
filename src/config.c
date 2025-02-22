#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

Config load_config(const char* config_path) {
    Config config = {
        .width = 0,
        .height = 0,
        .wallpaper_path = NULL
    };

    FILE* file = fopen(config_path, "r");
    if (!file) return config;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char* delim = strchr(line, '=');
        if (!delim) continue;
        
        *delim = '\0';
        char* key = line;
        char* value = delim + 1;
        value[strcspn(value, "\n")] = '\0';

        if (strcmp(key, "resolution") == 0) {
            sscanf(value, "%dx%d", &config.width, &config.height);
        }
        else if (strcmp(key, "wallpaper") == 0) {
            if (strcmp(value, "null") != 0) {
                config.wallpaper_path = strdup(value);
            }
        }
    }

    fclose(file);
    return config;
}

void free_config(Config* config) {
    if (config->wallpaper_path) {
        free(config->wallpaper_path);
    }
}
