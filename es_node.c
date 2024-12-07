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
	node->sk = -1;
}

void
es_fini(es_node *node)
{
	close(node->sk);
}

es_status
es_bind(es_node *node, uint16_t bind_port)
{
	struct sockaddr_in addr = { 0 };
	int ret;
	int sk;
	es_status rc;

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0)
	{
		rc = ES_ECONNFAIL;
		goto err;
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(bind_port);

	ret = bind(sk, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0)
	{
		rc = ES_ESKBINDFAIL;
		goto err;
	}

	ret = fcntl(sk, F_GETFD);
	if (ret == -1 || fcntl(sk, F_SETFD, ret | FD_CLOEXEC) != 0)
	{
		rc = ES_ESKCLOEXECFAIL;
		goto err;
	}

	ret = fcntl(sk, F_GETFL);
	if (ret == -1 || fcntl(sk, F_SETFL, ret | O_NONBLOCK) != 0)
	{
		rc = ES_ESKNONBLOCKFAIL;
		goto err;
	}

	node->sk = sk;
	return ES_EOK;

err:
	close(sk);
	return rc;
}
