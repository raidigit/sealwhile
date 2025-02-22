#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>

#define MOD_KEY Mod1Mask    // Alt
#define SHIFT_MOD ShiftMask
#define QUIT_KEY XK_q

Display *dpy;
Window root;
Cursor cursor;

void cleanup() {
    XFreeCursor(dpy, cursor);
    XCloseDisplay(dpy);
}

int main() {
    dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open display\n");
        return 1;
    }

    screen = DefaultScreen(dpy);
    root = RootWindow(dpy, screen);

    // Черный фон
    XSetWindowBackground(dpy, root, BlackPixel(dpy, screen));
    XClearWindow(dpy, root);

    // Курсор (стрелка)
    cursor = XCreateFontCursor(dpy, XC_left_ptr);
    XDefineCursor(dpy, root, cursor);

    // Перехват горячих клавиш
    XGrabKey(dpy, 
        XKeysymToKeycode(dpy, QUIT_KEY), 
        MOD_KEY | SHIFT_MOD, 
        root, 
        True, 
        GrabModeAsync, 
        GrabModeAsync
    );

    // Обработка событий
    XEvent ev;
    XSelectInput(dpy, root, KeyPressMask);
    
    while (1) {
        XNextEvent(dpy, &ev);
        if (ev.type == KeyPress) {
            KeySym keysym = XLookupKeysym(&ev.xkey, 0);
            if (keysym == QUIT_KEY) {
                cleanup();
                return 0;
            }
        }
    }
}