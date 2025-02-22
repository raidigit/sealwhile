CC = gcc
CFLAGS = -Wall -Wextra -O2 $(shell pkg-config --cflags wlroots wayland-server xkbcommon)
LDFLAGS = $(shell pkg-config --libs wlroots wayland-server xkbcommon)
TARGET = tuskwm
SRC = main.c
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

.PHONY: all clean install uninstall
