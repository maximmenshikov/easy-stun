#include "es_node.h"
#include "es_params.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "stun.h"
#include "debug.h"

int main(int argc, const char *argv[])
{
    es_status rc;
    uint16_t local_port;
    es_node *node = calloc(1, sizeof(es_node));
    es_params *params = calloc(1, sizeof(es_params));

    rc = es_params_read_from_cmdline(params, argc, argv);
    if (rc != ES_EOK)
    {
        crit("Invalid command line arguments");
        return -1;
    }

    rc = es_params_read_config(params);
    if (rc != ES_EOK)
    {
        crit("Invalid config parameters");
        return -1;
    }

    es_init(node);
    es_init_params(node, params);
    rc = es_twoway_bind(node);
    if (rc != ES_EOK)
    {
        crit("Failed to bind");
        return -1;
    }

    rc = es_local_start_recv(node);
    if (rc != ES_EOK)
    {
        crit("Failed to start receiving");
        return -1;
    }

    while (ES_TRUE)
    {
        pause();
    }
    es_fini(node);

    return 0;
}
