#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "es_params.h"
#include "es_status.h"

#define BIT(_num) (1 << (_num))

es_status
es_params_init(es_params *params, int argc, const char *argv[])
{
    size_t i;
    const int total_params = 5;
    int param_bits = 0;

    memset(params, 0, sizeof(es_params));

    for (i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--local-port") == 0)
        {
            if ((i + 1) >= argc)
                return ES_EPARAMMISSING;

            params->local_port = atoi(argv[i + 1]);
            param_bits |= BIT(0);

            ++i;
        }
        else if (strcmp(argv[i], "--remote-addr") == 0)
        {
            if ((i + 1) >= argc)
                return ES_EPARAMMISSING;

            if (strlen(argv[i + 1]) + 1 >= sizeof(params->remote_addr))
                return ES_EPARAMINVALID;

            strcpy(params->remote_addr, argv[i + 1]);
            param_bits |= BIT(1);

            ++i;
        }
        else if (strcmp(argv[i], "--remote-port") == 0)
        {
            if ((i + 1) >= argc)
                return ES_EPARAMMISSING;

            params->remote_port = atoi(argv[i + 1]);
            param_bits |= BIT(2);

            ++i;
        }
        else if (strcmp(argv[i], "--username") == 0)
        {
            if ((i + 1) >= argc)
                return ES_EPARAMMISSING;

            if (strlen(argv[i + 1]) + 1 >= sizeof(params->username))
                return ES_EPARAMINVALID;

            strcpy(params->username, argv[i + 1]);
            param_bits |= BIT(3);

            ++i;
        }
        else if (strcmp(argv[i], "--password") == 0)
        {
            if ((i + 1) >= argc)
                return ES_EPARAMMISSING;

            if (strlen(argv[i + 1]) + 1 >= sizeof(params->password))
                return ES_EPARAMINVALID;

            strcpy(params->password, argv[i + 1]);
            param_bits |= BIT(4);

            ++i;
        }
    }

    if (param_bits !=  ((1 << total_params) - 1))
    {
        return ES_EPARAMINSUFFICIENT;
    }

    return ES_EOK;
}