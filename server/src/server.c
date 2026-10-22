#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <microhttpd.h>
#include <jansson.h>

#include "server.h"
#include "config.h"
#include "utils.h"

static enum MHD_Result handle_request(
    void *cls,
    struct MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **connection_ctx
) {
    (void)cls;
    (void)url;
    (void)version;
    (void)upload_data;

    if (*connection_ctx == NULL) {
        *connection_ctx = connection;
        return MHD_YES;
    }

    if (*upload_data_size != 0) {
        *upload_data_size = 0;
        return MHD_YES;
    }

    if (strcmp(method, MHD_HTTP_METHOD_GET) != 0 &&
        strcmp(method, MHD_HTTP_METHOD_HEAD) != 0) {
        return MHD_NO;
    }

    char* body;
    int app_id = sceSystemServiceGetAppIdOfRunningBigApp();

    if (app_id < 0) {
        body = rpc_format_body("idle", NULL);
    } else {
        char title_id[16] = {0};

        if (sceSystemServiceGetAppTitleId(app_id, &title_id[0]) != 0) {
            body = rpc_format_body("unknown", NULL);
        } else {
            body = rpc_format_body("playing", title_id);
        }
    }

    if (body == NULL) {
        return MHD_NO;
    }

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(body), (void*)body, MHD_RESPMEM_MUST_FREE);

    if (response == NULL) {
        free(body);
        return MHD_NO;
    }

    MHD_add_response_header(response, "Content-Type", "application/json");
    enum MHD_Result result = MHD_queue_response(
        connection,
        MHD_HTTP_OK,
        response
    );

    MHD_destroy_response(response);
    return result;
}

char* rpc_format_body(char* state, char* title_id) {
    json_t* root = json_pack(
        // https://jansson.readthedocs.io/en/latest/apiref.html#building-values
        "{s:s, s:o}",
        "state", state,
        "title_id", title_id
            ? json_string(title_id)
            : json_null()
    );

    if (root == NULL) {
        notifyf("ps5-discord-rpc: request body creation failed! this is a bug!");
        return NULL;
    }

    char* str = json_dumps(root, JSON_INDENT(4));
    json_decref(root);

    if (str == NULL) {
        notifyf("ps5-discord-rpc: JSON serialization failed! this is a bug!");
        return NULL;
    }

    return str;
}

int rpc_listen(config_t* config)
{
    signal(SIGPIPE, SIG_IGN);

    int port = config->port;
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD, port, NULL, NULL, &handle_request, NULL, MHD_OPTION_END);

    if (daemon == NULL) {
        notifyf("ps5-discord-rpc failed to start! is something already running on port %d?", port);
        return -1;
    }

    notifyf("ps5-discord-rpc by azurejelly\nlistening on port %d", port);

    for (;;) {
        pause();
    }

    MHD_stop_daemon(daemon);
    return 0;
}