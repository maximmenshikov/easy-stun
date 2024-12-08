#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "es_params.h"
#include "es_status.h"

#define BIT(_num) (1 << (_num))

static es_status
parse_argument(es_params *params, const char *var, const char *value)
{
    #define PARSE_PORT(__name, __struct_name, __bit) do {                   \
        if (strcmp(var, __name) == 0)                                       \
        {                                                                   \
            if (strcmp(value, "any") == 0)                                  \
                params->__struct_name = 0;                                  \
            else                                                            \
                params->__struct_name = atoi(value);                        \
            params->present_fields |= BIT(__bit);                           \
            return ES_EOK;                                                  \
        }                                                                   \
    } while (0)
    #define PARSE_TIMEOUT(__name, __struct_name, __bit) do {                \
        if (strcmp(var, __name) == 0)                                       \
        {                                                                   \
            params->__struct_name = atoi(value);                            \
            params->present_fields |= BIT(__bit);                           \
            return ES_EOK;                                                  \
        }                                                                   \
    } while (0)
    #define PARSE_STR(__name, __struct_name, __bit) do {                    \
        if (strcmp(var, __name) == 0)                                       \
        {                                                                   \
            if (strlen(value) >= sizeof(params->__struct_name))             \
                return ES_EPARAMINVALID;                                    \
            strcpy(params->__struct_name, value);                           \
            params->present_fields |= BIT(__bit);                           \
            return ES_EOK;                                                  \
        }                                                                   \
    } while (0)

    PARSE_PORT("local-port", local_port, 0);
    PARSE_STR("remote-addr", remote_addr, 1);
    PARSE_PORT("remote-port", remote_port, 2);
    PARSE_STR("username", username, 3);
    PARSE_STR("password", password, 4);
    PARSE_STR("script", script, 5);
    PARSE_TIMEOUT("keepalive-interval", keepalive_interval, 6);

    #undef PARSE_PORT
    #undef PARSE_TIMEOUT
    #undef PARSE_STR
    return ES_EINVAL;
}

es_status
es_params_read_config(es_params *params)
{
    es_status rc;
    char line[1024];
    FILE *f;

    f = fopen(params->config, "r");
    if (f == NULL)
        return ES_EIO;

    rc = ES_EOK;
    while (fgets(line, sizeof(line), f) != NULL)
    {
        char *cmd_end = strchr(line, ' ');
        char *command;
        char *rest;

        if (cmd_end == NULL)
            return ES_EINVAL;

        *cmd_end = '\0';
        command = line;
        rest = cmd_end + 1;

        cmd_end = strchr(rest, '\n');
        if (cmd_end != NULL)
            *cmd_end = '\0';

        cmd_end = strchr(rest, '\r');
        if (cmd_end != NULL)
            *cmd_end = '\0';

        rc = parse_argument(params, command, rest);
        if (rc != ES_EOK)
            return rc;
    }

    fclose(f);

    if (params->present_fields !=  ((1 << ES_TOTAL_PARAMS) - 1))
    {
        return ES_EPARAMINSUFFICIENT;
    }

    return ES_EOK;
}

es_status
es_params_read_from_cmdline(es_params *params, int argc, const char *argv[])
{
    size_t i;
    const int total_params = 5;
    int param_bits = 0;

    memset(params, 0, sizeof(es_params));

    for (i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--config") == 0)
        {
            if ((i + 1) >= argc)
                return ES_EPARAMMISSING;

            if (strlen(argv[i + 1]) + 1 >= sizeof(params->config))
                return ES_EPARAMINVALID;

            strcpy(params->config, argv[i + 1]);

            ++i;
        }
    }

    return ES_EOK;
}