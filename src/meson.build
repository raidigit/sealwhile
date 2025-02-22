project('tuskwm', 'c', version: '0.1')

wlroots = dependency('wlroots')
wayland_server = dependency('wayland-server')
xkbcommon = dependency('xkbcommon')

executable(
    'tuskwm',
    'main.c',
    dependencies: [wlroots, wayland_server, xkbcommon],
    install: true,
)
