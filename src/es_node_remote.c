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
#include "es_msg.h"
#include "debug.h"

es_status
es_remote_bind(es_node *node)
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

    host = gethostbyname(node->params.remote_addr);
    if (host == NULL)
    {
        rc = ES_EDNSFAIL;
        goto err;
    }

    addr.sin_port = htons(node->params.remote_port);
    addr.sin_family = AF_INET;
    memcpy(&(addr.sin_addr), host->h_addr_list[0], host->h_length);

    es_msg_hdr_init(&msg, 0x0001);
    EXIT_ON_ERROR("Failed to add username",
                  es_msg_add_attr(&msg,
                                  STUN_ATTR_USERNAME,
                                  node->params.username,
                                  strlen(node->params.username)));
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
                  es_msg_add_integrity(&msg, node->params.password));

    msg.hdr->message_len = htons(msg.hdr->message_len);
    ret = sendto(node->sk, req, ntohs(msg.hdr->message_len) + sizeof(stun_hdr),
        0, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        err("Failed to send message: %s", strerror(errno));
        rc = ES_ESENDFAIL;
        goto err;
    }

    es_init_status(node, ES_MAP_STATUS_SENT);
    es_expect_tid(node, msg.hdr->tid);

    ring("[%s:%u] Sent request with %d bytes",
         node->params.remote_addr, (unsigned)node->params.remote_port,
         ret);
    return ES_EOK;

err:
    return rc;
}
