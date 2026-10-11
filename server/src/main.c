#include "server.h"
#include "utils.h"

#define SERVER_PORT 7270

int main(void) {
    //todo: coulld add a config file that allows to blacklist certain adds from showing up and to change the webserver port
    return rpc_listen(SERVER_PORT);
}
