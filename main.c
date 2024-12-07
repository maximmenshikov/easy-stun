
#include "es_node.h"
#include "es_status.h"



int main()
{
	es_status rc;
	uint16_t bind_port = 32300;
	es_node node;

	es_init(&node);
	es_bind(&node, bind_port);
	es_fini(&node);

	return 0;
}
