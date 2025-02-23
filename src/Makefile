CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lX11 -lImlib2
TARGET = tuskwm
SRC = main.c config.c
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

clean:
	rm -f $(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) $(PREFIX)/bin/$(TARGET)
	install -Dm644 /etc/tuskwm.conf $(PREFIX)/etc/tuskwm.conf

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)
	rm -f $(PREFIX)/etc/tuskwm.conf

.PHONY: all clean install uninstall
