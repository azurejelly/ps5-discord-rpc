#pragma once

#include "config.h"

char* rpc_format_body(char* state, char* title_id);

int rpc_listen(config_t* config);
