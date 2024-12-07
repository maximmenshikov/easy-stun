#include "es_node.h"

int main()
{
	es_status rc;
	uint16_t bind_port = 32300;
	es_node node;

	es_init(&node);
	es_local_bind(&node, bind_port);
	es_remote_bind(&node, "", 3478);
	es_fini(&node);

	return 0;
}
