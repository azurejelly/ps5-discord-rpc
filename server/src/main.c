#include "ini.h"

#include "server.h"
#include "config.h"
#include "utils.h"

#define DEFAULT_SERVER_PORT 7270

int main(void) {
    config_t cfg = {
        .version = CONFIG_VERSION,
        .port = DEFAULT_SERVER_PORT
    };

    if (rpc_init_config(&cfg) != 0) {
        return -1;
    }

    return rpc_listen(&cfg);
}
