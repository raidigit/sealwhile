#include <wayland-server.h>
#include <wlr/backend.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>

struct tuskwm_server {
    struct wl_display *wl_display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_output_layout *output_layout;
    struct wlr_cursor *cursor;
    struct wlr_seat *seat;
    
    struct wl_listener new_output;
    struct wl_listener new_input;
    struct wl_listener cursor_motion;
    struct wl_listener keyboard_key;
};

static void handle_output_new(struct wl_listener *listener, void *data) {
    struct tuskwm_server *server = wl_container_of(listener, server, new_output);
    struct wlr_output *output = data;
    
    if (!wlr_output_init_render(output, server->renderer, NULL)) {
        wlr_output_attach_render(output, NULL);
        wlr_output_commit(output);
    }
    
    wlr_output_layout_add_auto(server->output_layout, output);
}

static void handle_cursor_motion(struct wl_listener *listener, void *data) {
    struct tuskwm_server *server = wl_container_of(listener, server, cursor_motion);
    struct wlr_event_pointer_motion *event = data;
    wlr_cursor_move(server->cursor, &event->pointer->base, event->delta_x, event->delta_y);
}

static void handle_keyboard_key(struct wl_listener *listener, void *data) {
    struct tuskwm_server *server = wl_container_of(listener, server, keyboard_key);
    struct wlr_event_keyboard_key *event = data;
    
    xkb_mod_mask_t mods = wlr_keyboard_get_modifiers(server->seat->keyboard_state.keyboard);
    if (mods == (WLR_MODIFIER_ALT | WLR_MODIFIER_SHIFT) && event->keycode == KEY_Q) {
        wl_display_terminate(server->wl_display);
    }
}

static void handle_input_new(struct wl_listener *listener, void *data) {
    struct tuskwm_server *server = wl_container_of(listener, server, new_input);
    struct wlr_input_device *device = data;
    
    switch (device->type) {
    case WLR_INPUT_DEVICE_KEYBOARD: {
        struct wlr_keyboard *keyboard = wlr_keyboard_from_input_device(device);
        wlr_keyboard_set_keymap(keyboard, xkb_keymap_new_from_names(server->seat->keyboard_state.xkb_context, NULL));
        server->keyboard_key.notify = handle_keyboard_key;
        wl_signal_add(&keyboard->events.key, &server->keyboard_key);
        break;
    }
    case WLR_INPUT_DEVICE_POINTER:
        wlr_cursor_attach_input_device(server->cursor, device);
        server->cursor_motion.notify = handle_cursor_motion;
        wl_signal_add(&server->cursor->events.motion, &server->cursor_motion);
        break;
    default:
        break;
    }
}

int main(int argc, char **argv) {
    struct tuskwm_server server = {0};
    
    server.wl_display = wl_display_create();
    server.backend = wlr_backend_autocreate(server.wl_display);
    server.renderer = wlr_renderer_autocreate(server.backend);
    wlr_renderer_init_wl_display(server.renderer, server.wl_display);
    
    server.output_layout = wlr_output_layout_create();
    server.new_output.notify = handle_output_new;
    wl_signal_add(&server.backend->events.new_output, &server.new_output);
    
    server.cursor = wlr_cursor_create();
    server.seat = wlr_seat_create(server.wl_display, "seat0");
    
    server.new_input.notify = handle_input_new;
    wl_signal_add(&server.backend->events.new_input, &server.new_input);
    
    const char *socket = wl_display_add_socket_auto(server.wl_display);
    if (!wlr_backend_start(server.backend)) {
        wlr_log(WLR_ERROR, "Failed to start backend");
        return 1;
    }
    
    wl_display_run(server.wl_display);
    wl_display_destroy(server.wl_display);
    return 0;
}
