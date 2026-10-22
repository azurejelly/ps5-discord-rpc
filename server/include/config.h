#pragma once

#include <string.h>

#define CONFIG_VERSION 1
#define CONFIG_FOLDER "/data/ps5-discord-rpc/"
#define CONFIG_FILE "config.ini"
#define CONFIG_LOCATION CONFIG_FOLDER CONFIG_FILE

static const char DEFAULT_CONFIG[] =
    "[config]\n"
    "version=1\n"
    "\n"
    "[server]\n"
    "port=7270\n";

typedef struct {
    int version;
    int port;
} config_t;

int rpc_init_config(config_t* cfg);

int rpc_config_handler(void* v, const char* section, const char* name, const char* value);

int rpc_has_config(void);

int rpc_write_default_config(void);