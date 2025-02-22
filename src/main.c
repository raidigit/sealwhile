#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include <stdlib.h>
#include <stdio.h>

Display* display;
Window root;
int screen;

void initX() {
    display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open X11 display\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    root = RootWindow(display, screen);

    XSetWindowAttributes attrs;
    attrs.background_pixel = BlackPixel(display, screen);
    XChangeWindowAttributes(display, root, CWBackPixel, &attrs);

    Cursor cursor = XCreateFontCursor(display, XC_left_ptr);
    XDefineCursor(display, root, cursor);

    XSelectInput(display, root, ExposureMask);
}

void eventLoop() {
    XEvent event;
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            XClearWindow(display, root);
        }
    }
}

int main() {
    initX();
    eventLoop();
    return 0;
}
