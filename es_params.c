#include <stddef.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include "es_params.h"
#include "es_status.h"

es_status
es_params_init(es_params *params, int argc, const char *argv[])
{
	size_t i;
	const int total_params = 3;
	int param_bits = 0;

	for (i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--local-port") == 0)
		{
			if ((i + 1) >= argc)
				return ES_EPARAMMISSING;

			params->local_port = atoi(argv[i + 1]);
			param_bits |= 1;

			++i;
		}
		else if (strcmp(argv[i], "--remote-addr") == 0)
		{
			if ((i + 1) >= argc)
				return ES_EPARAMMISSING;

			if (strlen(argv[i + 1]) + 1 >= sizeof(params->remote_addr))
				return ES_EPARAMINVALID;

			strcpy(params->remote_addr, argv[i + 1]);
			param_bits |= 2;

			++i;
		}
		else if (strcmp(argv[i], "--remote-port") == 0)
		{
			if ((i + 1) >= argc)
				return ES_EPARAMMISSING;

			params->remote_port = atoi(argv[i + 1]);
			param_bits |= 4;

			++i;
		}
	}

	if (param_bits !=  ((1 << total_params) - 1))
	{
		return ES_EPARAMINSUFFICIENT;
	}

	return ES_EOK;
}