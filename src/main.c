#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <Imlib2.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"

Display* display;
Window root;
int screen;
Config app_config;

void set_wallpaper(const char* path) {
    if (!path) {
        XSetWindowBackground(display, root, BlackPixel(display, screen));
        XClearWindow(display, root);
        return;
    }

    Imlib_Image image = imlib_load_image(path);
    if (!image) return;

    imlib_context_set_image(image);
    int width = imlib_image_get_width();
    int height = imlib_image_get_height();

    Pixmap pixmap = XCreatePixmap(display, root, width, height, DefaultDepth(display, screen));
    imlib_context_set_display(display);
    imlib_context_set_visual(DefaultVisual(display, screen));
    imlib_context_set_colormap(DefaultColormap(display, screen));
    imlib_context_set_drawable(pixmap);
    imlib_render_image_on_drawable(0, 0);

    XSetWindowBackgroundPixmap(display, root, pixmap);
    XClearWindow(display, root);
    imlib_free_image();
    XFreePixmap(display, pixmap);
}

void init_x() {
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

    // Apply resolution if configured
    if (app_config.width > 0 && app_config.height > 0) {
        XResizeWindow(display, root, app_config.width, app_config.height);
    }

    // Set cursor
    Cursor cursor = XCreateFontCursor(display, XC_left_ptr);
    XDefineCursor(display, root, cursor);

    // Set background
    set_wallpaper(app_config.wallpaper_path);
}

void event_loop() {
    XEvent ev;
    while (1) {
        XNextEvent(display, &ev);
        if (ev.type == Expose) {
            XClearWindow(display, root);
        }
    }
}

int main() {
    app_config = load_config();
    imlib_set_cache_size(1024 * 1024);
    imlib_set_color_usage(256);
    
    init_x();
    event_loop();
    
    free_config(&app_config);
    XCloseDisplay(display);
    return 0;
}
