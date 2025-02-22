#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    int width;
    int height;
    char* wallpaper_path;
} Config;

Config load_config(const char* config_path);
void free_config(Config* config);

#endif
