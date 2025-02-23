#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config load_config() {
    Config config = {0, 0, NULL};

    FILE* file = fopen(CONFIG_PATH, "r");
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
