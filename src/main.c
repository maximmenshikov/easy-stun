#include "es_node.h"
#include "es_params.h"
#include <stdio.h>

int main(int argc, const char *argv[])
{
	es_status rc;
	uint16_t local_port;
	es_node node;
	es_params params;


	rc = es_params_init(&params, argc, argv);
	if (rc != ES_EOK)
	{
		printf("Invalid parameters");
		return -1;
	}

	es_init(&node);
	es_local_bind(&node, &params);
	es_remote_bind(&node, &params);
	es_fini(&node);

	return 0;
}
