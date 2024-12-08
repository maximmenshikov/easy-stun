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
#include <errno.h>
#include "es_node.h"
#include "es_status.h"
#include "es_bool.h"
#include "stun.h"
#include <openssl/ssl.h>
#include "helper.h"
#include "debug.h"

typedef struct es_msg {
	size_t max_len;
	stun_hdr *hdr;
} es_msg;

static es_bool
fits(es_msg *msg, uint32_t appended_len)
{
	return (msg->hdr->message_len + sizeof(*msg->hdr) + appended_len) <= msg->max_len;
}

static void
es_msg_hdr_init(es_msg *msg, uint16_t type)
{
	msg->hdr->message_type = htons(type);
	msg->hdr->magic_cookie = htonl(STUN_MAGIC_COOKIE);
    for (int i = 0; i < sizeof(msg->hdr->tid); ++i)
    {
        msg->hdr->tid[i] = (uint8_t)(rand() % 256);
    }

    msg->hdr->message_len = 0;
}

static es_status
es_msg_add_attr(es_msg *msg, stun_attr_type type, void *value, uint32_t value_len)
{
	stun_attr *attr;
	size_t padding_len;
	int old_msg_len = msg->hdr->message_len;

	UNUSED(old_msg_len);

	padding_len = PAD(value_len, 4);

	if (!fits(msg, sizeof(stun_attr) + value_len + padding_len))
	{
		err("Couldn't fit msg=%lu+%d appended=%lu+%d padding=%lu",
			sizeof(*msg->hdr), msg->hdr->message_len,
			sizeof(stun_attr), value_len, padding_len);
		return ES_ENOMEM;
	}

	attr = (stun_attr *)&msg->hdr->data[msg->hdr->message_len];
	attr->type = htons(type);
	attr->length = htons(value_len);
	memset(attr->value, 0, value_len + padding_len);
	memcpy(attr->value, value, value_len);

	msg->hdr->message_len += sizeof(stun_attr) + value_len + padding_len;
	dbg("Fit attr type %d msg=%lu+(%d->%d) appended=%lu+%d padding=%lu",
		type, sizeof(*msg->hdr),
		old_msg_len,
		msg->hdr->message_len,
		sizeof(stun_attr), value_len, padding_len);
	return ES_EOK;
}

static es_status
es_msg_add_integrity(es_msg *msg, es_params *params)
{
	uint32_t msg_len = msg->hdr->message_len + sizeof(*msg->hdr);
	uint32_t padding = PAD(msg_len, 64);
	unsigned char *digest;

	if (msg_len + padding >= msg->max_len)
		return ES_ENOMEM;

	digest = HMAC(EVP_sha1(), params->password, strlen(params->password),
		(const unsigned char *)msg->hdr, msg_len + padding, NULL, NULL);
	return es_msg_add_attr(msg, STUN_ATTR_MESSAGE_INTEGRITY, digest, 20);
}

es_status
es_remote_bind(es_node *node, es_params *params)
{
	es_status rc;
	struct hostent *host;
	struct sockaddr_in addr;
	char req[4096] = { 0 };
	int ret;
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

	es_msg_hdr_init(&msg, 0x0001);
    EXIT_ON_ERROR("Failed to add username",
    	          es_msg_add_attr(&msg,
    	                          STUN_ATTR_USERNAME,
    	                          params->username,
    	                          strlen(params->username)));
    EXIT_ON_ERROR("Failed to add connection request binding",
    	          es_msg_add_attr(&msg,
    	                          STUN_ATTR_CONN_REQUEST_BINDING,
    	                          "dslforum.org/TR-111 ",
    	                          20));
    EXIT_ON_ERROR("Failed to add binding change",
    	          es_msg_add_attr(&msg,
    	                          STUN_ATTR_BINDING_CHANGE,
    	                          "",
    	                          0));
    EXIT_ON_ERROR("Failed to add message integrity",
    	          es_msg_add_integrity(&msg, params));

    msg.hdr->message_len = htons(msg.hdr->message_len);
	ret = sendto(node->sk, req, ntohs(msg.hdr->message_len) + sizeof(stun_hdr), 0,
		(struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0)
	{
		err("Failed to send message: %s", strerror(errno));
		rc = ES_ESENDFAIL;
		goto err;
	}

	dbg("Sent %d bytes", ret);
	return ES_EOK;

err:
	return rc;
}
