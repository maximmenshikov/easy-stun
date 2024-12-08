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
