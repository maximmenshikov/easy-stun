#pragma once
#include <inttypes.h>
#include "es_status.h"

typedef struct es_params
{
	uint16_t local_port;
	char remote_addr[512];
	char username[512];
	char password[512];
	uint16_t remote_port;
} es_params;

extern es_status es_params_init(es_params *params, int argc, const char *argv[]);
