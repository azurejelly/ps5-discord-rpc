PS5_HOST ?= ps5
PS5_PORT ?= 9021

ifdef PS5_PAYLOAD_SDK
	include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
	$(error PS5_PAYLOAD_SDK is undefined)
endif

OUT_DIR := build
ELF := ps5-discord-rpc.elf

SRCS := src/main.c src/server.c src/utils.c

CFLAGS := -Wall -Werror -g

LDADD := -L $(HOMEBREW)/lib -lSceSystemService -lmicrohttpd -lpthread -ljansson

all: $(ELF)

$(ELF): $(SRCS)
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) -o $(OUT_DIR)/$@ $(SRCS) $(LDADD)

clean:
	rm -rf $(OUT_DIR)

test: $(ELF)
	$(PS5_DEPLOY) -h $(PS5_HOST) -p $(PS5_PORT) $^

debug: $(ELF)
	gdb-multiarch \
	-ex "set architecture i386:x86-64" \
	-ex "target extended-remote $(PS5_HOST):2159" \
	-ex "file $(ELF)" \
	-ex "remote put $(ELF) /data/$(ELF)" \
	-ex "set remote exec-file /data/$(ELF)" \
	-ex "start"
