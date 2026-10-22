#pragma once

#include <string.h>

// notify code is mostly from the ps5-payload-sdk samples
// https://github.com/ps5-payload-dev/sdk/blob/f5d40f02770e9b0001dd9e18056eca70b3da8b40/samples/notify_debug/main.c
typedef struct notify_request {
  char useless1[45];
  char message[3075];
} notify_request_t;

int sceKernelSendNotificationRequest(int, notify_request_t*, size_t, int);

int sceSystemServiceGetAppIdOfRunningBigApp(void);

int sceSystemServiceGetAppTitleId(int app_id, char *title_id);

void notifyf(const char* fmt, ...);