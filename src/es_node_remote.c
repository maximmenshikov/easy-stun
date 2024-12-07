#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "es_node.h"
#include "es_status.h"

es_status
es_remote_bind(es_node *node, const char *srv_addr, uint16_t server_port)
{
	es_status rc;
	struct hostent *host;
	struct sockaddr_in addr;
	char req[4096];
	size_t req_len = 0;
	int ret;

	host = gethostbyname(srv_addr);
	if (host == NULL)
	{
		rc = ES_EDNSFAIL;
		goto err;
	}

	addr.sin_port = htons(server_port);
	addr.sin_family = AF_INET;
	memcpy(&(addr.sin_addr), host->h_addr_list[0], host->h_length);

	ret = sendto(node->sk, req, req_len, 0,
		(struct sockaddr *)&addr, sizeof(addr));
	if (ret != 0)
	{
		rc = ES_ESENDFAIL;
		goto err;
	}

	return ES_EOK;

err:
	return rc;
}
