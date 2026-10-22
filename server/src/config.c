#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "ini.h"
#include "config.h"
#include "utils.h"

int rpc_init_config(config_t* cfg) {
    if (!rpc_has_config() && rpc_write_default_config() != 0) {
        notifyf("ps5-discord-rpc: failed to write default config! the payload will now exit.");
        return -1;
    }

    if (ini_parse(CONFIG_LOCATION, rpc_config_handler, cfg) < 0) {
        notifyf("ps5-discord-rpc: failed to parse config - please check for any syntax errors and resend the payload.");
        return -1;
    }

    if (cfg->port < 1 || cfg->port > 65535) {
        notifyf("ps5-discord-rpc: invalid server port provided! must be a number between 1-65535. the payload will now exit.");
        return -1;
    }

    if (cfg->version > CONFIG_VERSION) {
        notifyf("ps5-discord-rpc: unsupported config version %d. the payload will now exit.", cfg->version);
        return -1;
    }

    return 0;
}

int rpc_config_handler(void* v, const char* section, const char* name, const char* value) {
    config_t* cfg = (config_t*) v;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

    if (MATCH("config", "version")) {
        cfg->version = atoi(value);
    }
    else if (MATCH("server", "port")) {
        cfg->port = atoi(value);
    } else {
        return 0;
    }

    return 1;
}

int rpc_has_config(void) {
    if (access(CONFIG_LOCATION, F_OK) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int rpc_write_default_config(void) {
    if (mkdir(CONFIG_FOLDER, 0755) != 0 && errno != EEXIST) {
        return -1;
    }

    FILE* file = fopen(CONFIG_LOCATION, "wx");
    if (!file) {
        return -1;
    }

    size_t length = sizeof(DEFAULT_CONFIG) - 1;
    int result = fwrite(DEFAULT_CONFIG, 1, length, file) == length ? 0 : -1;

    if (fclose(file) != 0) {
        result = -1;
    }

    return result;
}
