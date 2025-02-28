#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_PATH "/etc/sealwhile.conf"

typedef struct {
    int width;
    int height;
    char* wallpaper_path;
} Config;

Config load_config();
void free_config(Config* config);

#endif
