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
#include "es_msg.h"

static es_bool
fits(es_msg *msg, uint32_t appended_len)
{
    return (msg->hdr->message_len + sizeof(*msg->hdr) + appended_len) <= msg->max_len;
}

void
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

es_status
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

es_status
es_msg_add_integrity(es_msg *msg, const char *password)
{
    uint32_t msg_len = msg->hdr->message_len + sizeof(*msg->hdr);
    uint32_t padding = PAD(msg_len, 64);
    unsigned char *digest;

    if (msg_len + padding >= msg->max_len)
        return ES_ENOMEM;

    digest = HMAC(EVP_sha1(), password, strlen(password),
        (const unsigned char *)msg->hdr, msg_len + padding, NULL, NULL);
    return es_msg_add_attr(msg, STUN_ATTR_MESSAGE_INTEGRITY, digest, 20);
}

es_status
es_msg_read_next_attr(es_msg *msg, stun_attr **attr, size_t *offset)
{
    stun_attr *tmp;

    if ((*offset + sizeof(stun_attr)) > msg->hdr->message_len)
    {
        return ES_ENODATA;
    }

    tmp = (stun_attr *)&msg->hdr->data[*offset];
    if (*offset + sizeof(stun_attr) + ntohs(tmp->length) > msg->hdr->message_len)
    {
        return ES_ENODATA;
    }

    *attr = tmp;
    *offset = *offset + sizeof(stun_attr) + ntohs(tmp->length);

    return ES_EOK;
}

es_status
es_msg_read_attr(es_msg *msg, stun_attr_type type, stun_attr **attr)
{
    size_t offset = 0;
    stun_attr *tmp;

    while (es_msg_read_next_attr(msg, &tmp, &offset) == ES_EOK)
    {
        if (ntohs(tmp->type) == type)
        {
            *attr = tmp;
            return ES_EOK;
        }
    }

    return ES_ENODATA;
}
