CC=gcc
CFLAGS=-Iinclude
LIBS=-lcurl -lssl -lcrypto

all:
	$(CC) $(CFLAGS) src/main.c src/ota_http.c src/downloader.c src/verifier.c src/slot_manager.c -o ota-daemon $(LIBS)
