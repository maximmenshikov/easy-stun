#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "es_node.h"
#include "es_status.h"
#include "es_bool.h"
#include "stun.h"
#include <openssl/ssl.h>

typedef struct es_msg {
	size_t max_len;
	stun_hdr *hdr;
} es_msg;

static es_bool
fits(es_msg *msg, uint32_t appended_len)
{
	return (msg->hdr->message_len + sizeof(*msg->hdr) + appended_len) <= msg->max_len;
}

static es_status
add_attr(es_msg *msg, stun_attr_type type, void *value, uint32_t value_len)
{
	stun_attr *attr;
	size_t padding_len;

	padding_len = 4 - (value_len % 4);

	if (!fits(msg, sizeof(stun_attr) + value_len + padding_len))
		return ES_ENOMEM;

	attr = (stun_attr *)&msg->hdr->data[msg->hdr->message_len];
	attr->type = type;
	attr->length = value_len;
	memset(attr->value, 0, value_len + padding_len);
	memcpy(attr->value, value, value_len);

	msg->hdr->message_len += sizeof(stun_attr) + value_len + padding_len;
	return ES_EOK;
}

static es_status
add_hash(es_msg *msg, es_params *params)
{
	uint32_t msg_len = msg->hdr->message_len + sizeof(*msg->hdr);
	uint32_t padding = 64 - (msg_len % 64);
	unsigned char *digest;

	if (msg_len + padding >= msg->max_len)
	{
		return ES_ENOMEM;
	}

	digest = HMAC(EVP_sha1(), params->password, strlen(params->password),
		(const unsigned char *)msg->hdr, msg_len + padding, NULL, NULL);
	return add_attr(msg, STUN_ATTR_MESSAGE_INTEGRITY, digest, 20);
}

es_status
es_remote_bind(es_node *node, es_params *params)
{
	es_status rc;
	struct hostent *host;
	struct sockaddr_in addr;
	char req[4096] = { 0 };
	int ret;
	stun_hdr *hdr;
	size_t offset;
	es_msg msg;

	msg.hdr = (stun_hdr *)req;
	msg.max_len = sizeof(req);

	host = gethostbyname(params->remote_addr);
	if (host == NULL)
	{
		rc = ES_EDNSFAIL;
		goto err;
	}

	addr.sin_port = htons(params->remote_port);
	addr.sin_family = AF_INET;
	memcpy(&(addr.sin_addr), host->h_addr_list[0], host->h_length);


	hdr = (stun_hdr *)req;
	hdr->message_type = htons(0x0001);
	hdr->magic_cookie = htonl(0x2112A442);
    for (int i = 0; i < sizeof(hdr->tid); ++i)
    {
        hdr->tid[i] = (uint8_t)(rand() % 256);
    }

    hdr->message_len = 0;

    printf("b %d\n", hdr->message_len);
    add_attr(&msg, STUN_ATTR_USERNAME, params->username, strlen(params->username));
    add_attr(&msg, STUN_ATTR_CONN_REQUEST_BINDING, "dslforum.org/TR-111 ", 20);
    add_attr(&msg, STUN_ATTR_BINDING_CHANGE, "", 0);
    add_hash(&msg, params);

    printf("a %d\n", hdr->message_len);
	ret = sendto(node->sk, req, hdr->message_len + sizeof(stun_hdr), 0,
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
