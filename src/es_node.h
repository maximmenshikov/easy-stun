#pragma once
#include "es_status.h"
#include "es_params.h"
#include "es_bool.h"
#include <inttypes.h>

typedef enum es_map_status_code
{
    ES_MAP_STATUS_NONE = 0,
    ES_MAP_STATUS_SENT,
    ES_MAP_STATUS_MAPPED,
    ES_MAP_STATUS_ERROR,
} es_map_status_code;

typedef struct es_node_status
{
    es_map_status_code code;

    /* in case of ES_MAP_STATUS_SENT */
    uint8_t  expected_tid[12];

    /* in case of ES_MAP_STATUS_MAPPED */
    char     mapped_addr[512];
    uint16_t mapped_port;

    /* in case of ES_MAP_STATUS_ERROR */
    uint16_t map_error;
} es_node_status;

typedef struct es_node
{
    int             sk;
    es_node_status  status;
} es_node;

extern void es_init(es_node *node);
extern void es_fini(es_node *node);
extern void es_init_status(es_node *node, es_map_status_code code);
extern es_status es_local_bind(es_node *node, es_params *params);
extern es_status es_local_recv(es_node *node);
extern es_status es_local_conn_request(es_node *node, const char *buf,
    uint32_t buf_len);
extern es_status es_remote_bind(es_node *node, es_params *params);
