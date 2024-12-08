#pragma once
#include "es_status.h"
#include "es_params.h"
#include "es_bool.h"
#include <inttypes.h>

typedef struct es_node
{
	int      sk;

	es_bool  mapped;
	char     mapped_addr[512];
	uint16_t mapped_port;
} es_node;

extern void es_init(es_node *node);
extern void es_fini(es_node *node);
extern es_status es_local_bind(es_node *node, es_params *params);
extern es_status es_local_recv(es_node *node);
extern es_status es_remote_bind(es_node *node, es_params *params);
