#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "es_node.h"
#include "es_status.h"

void
es_init(es_node *node)
{
    memset(node, 0, sizeof(es_node));
    node->sk = -1;
}

void
es_fini(es_node *node)
{
    close(node->sk);
}

void
es_init_status(es_node *node, es_map_status_code code)
{
    memset(&node->status, 0, sizeof(es_node_status));
    node->status.code = code;
}

void
es_init_params(es_node *node, es_params *params)
{
    memcpy(&node->params, params, sizeof(es_params));
}

void
es_expect_tid(es_node *node, uint8_t *tid)
{
    memcpy(node->status.expected_tid, tid, sizeof(node->status.expected_tid));
}
