#pragma once
#include "es_status.h"
#include <inttypes.h>

typedef struct es_node {
	int sk;
} es_node;

extern void es_init(es_node *node);
extern void es_fini(es_node *node);
extern es_status es_bind(es_node *node, uint16_t bind_port);
