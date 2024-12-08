#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "es_node.h"
#include "es_status.h"
#include "debug.h"

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

es_status
es_twoway_bind(es_node *node)
{
    es_status rc;

    if (node->sk != -1)
    {
        close(node->sk);
        node->sk = -1;
    }

    es_init_status(node, ES_MAP_STATUS_NONE);
    rc = es_local_bind(node);
    if (rc != ES_EOK)
    {
        err("Failed to bind locally");
        return rc;
    }

    return es_remote_bind(node);
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
