#include "es_node.h"
#include "es_params.h"
#include <stdio.h>
#include <unistd.h>
#include "stun.h"
#include "debug.h"

int main(int argc, const char *argv[])
{
    es_status rc;
    uint16_t local_port;
    es_node node;
    es_params params;

    rc = es_params_read_from_cmdline(&params, argc, argv);
    if (rc != ES_EOK)
    {
        crit("Invalid command line arguments");
        return -1;
    }

    rc = es_params_read_config(&params);
    if (rc != ES_EOK)
    {
        crit("Invalid config parameters");
        return -1;
    }

    es_init(&node);
    es_init_params(&node, &params);
    es_twoway_bind(&node);
    es_local_start_recv(&node);
    while (ES_TRUE)
    {
        pause();
    }
    es_fini(&node);

    return 0;
}
