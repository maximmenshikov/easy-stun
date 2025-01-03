#pragma once
#include <inttypes.h>
#include "es_status.h"

typedef struct es_params
{
    char config[512];
    uint32_t present_fields;
    es_bool fork;

#define ES_TOTAL_PARAMS (10)

    uint16_t local_port;
    char remote_addr[512];
    uint16_t remote_port;
    char username[512];
    char password[512];
    char script[512];
    uint32_t keepalive_interval;
    char acs_addr[512];
    uint16_t acs_port;
    uint32_t restart_interval;
} es_params;

extern es_status es_params_read_from_cmdline(es_params *params, int argc,
    const char *argv[]);
extern es_status es_params_read_config(es_params *params);
