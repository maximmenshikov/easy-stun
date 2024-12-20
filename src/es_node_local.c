#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "es_node.h"
#include "es_status.h"
#include "stun.h"
#include "es_msg.h"
#include "es_bool.h"
#include "debug.h"
#include "helper.h"

es_status
es_local_bind(es_node *node)
{
    struct sockaddr_in addr = { 0 };
    int ret;
    int sk;
    es_status rc;

    sk = socket(AF_INET, SOCK_DGRAM, 0);
    if (sk < 0)
    {
        dbg("Failed to open socket");
        rc = ES_ECONNFAIL;
        goto err;
    }

    if (node->params.local_port != 0)
    {
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(node->params.local_port);

        ret = bind(sk, (struct sockaddr *)&addr, sizeof(addr));
        if (ret < 0)
        {
            dbg("Failed to bind socket");
            rc = ES_ESKBINDFAIL;
            goto err;
        }
    }

    ret = fcntl(sk, F_GETFD);
    if (ret == -1 || fcntl(sk, F_SETFD, ret | FD_CLOEXEC) != 0)
    {
        dbg("Failed to change cloexec");
        rc = ES_ESKCLOEXECFAIL;
        goto err;
    }

    ret = fcntl(sk, F_GETFL);
    if (ret == -1 || fcntl(sk, F_SETFL, ret | O_NONBLOCK) != 0)
    {
        dbg("Failed to make socket non-blocking");
        rc = ES_ESKNONBLOCKFAIL;
        goto err;
    }

    node->sk = sk;
    return ES_EOK;

err:
    close(sk);
    return rc;
}

es_status
es_local_process_binding_response(es_node *node,
                                  es_msg *msg)
{
    es_status rc;
    stun_attr *attr;
    stun_attr_mapped_address *ma;
    es_bool xored = ES_FALSE;

    rc = es_msg_read_attr(msg, STUN_ATTR_MAPPED_ADDRESS, &attr);
    if (rc != ES_EOK || attr == NULL)
    {
        rc = es_msg_read_attr(msg, STUN_ATTR_XOR_MAPPED_ADDRESS, &attr);
        if (rc != ES_EOK || attr == NULL)
        {
            if (rc == ES_EOK && attr == NULL)
                rc = ES_ENODATA;
            err("%s: Attribute not found: mapped address", __func__);
            return rc;
        }

        xored = ES_TRUE;
    }

    ma = (stun_attr_mapped_address *)attr->value;

    es_init_status(node, ES_MAP_STATUS_MAPPED);

    switch (ma->family)
    {
        case STUN_AF_IPV4:
        {
            uint32_t val = ntohl(*(uint32_t *)(&ma->addr));
            uint32_t unxored = val;
            uint32_t port = ntohs(ma->port);
            struct in_addr ip;

            if (xored)
            {
                uint32_t warped_cookie = ntohl(msg->hdr->magic_cookie);
                uint16_t warped_cookie16 = (warped_cookie & 0xFFFF0000) >> 16;

                val = val ^ warped_cookie;
                port = port ^ warped_cookie16;
            }

            ip.s_addr = htonl(val);

            node->status.mapped_port = port;
            sprintf(node->status.mapped_addr, "%s", inet_ntoa(ip));
            ring("[%s:%u] Mapped to %s:%u",
                node->params.remote_addr,
                (unsigned)node->params.remote_port,
                node->status.mapped_addr,
                node->status.mapped_port);
            break;
        }
        case STUN_AF_IPV6:
        {
            err("%s: Attribute error: mapped address is IPv6 (unsupported)",
                __func__);
            return ES_ENOTSUPP;
        }
    }

    {
        char full_cmd[1024];
        int ret;

        sprintf(full_cmd, "%s bind %s %u", node->params.script,
            node->status.mapped_addr,
            (unsigned)node->status.mapped_port);
        ret = system(full_cmd);
        ring("Script '%s' executed with return code %d", node->params.script,
             ret);
    }
    return ES_EOK;
}

es_status
es_local_process_binding_error(es_node *node,
                               es_msg *msg)
{
    es_status rc;
    stun_attr *attr;
    stun_attr_error_code *ec;
    uint32_t cls_number;

    rc = es_msg_read_attr(msg, STUN_ATTR_ERROR_CODE, &attr);
    if (rc != ES_EOK || attr == NULL)
    {
        if (rc == ES_EOK && attr == NULL)
            rc = ES_ENODATA;
        return rc;
    }

    ec = (stun_attr_error_code *)attr->value;

    cls_number = ntohl(ec->cls_number);

    es_init_status(node, ES_MAP_STATUS_ERROR);
    node->status.map_error = ((cls_number >> 8) & 0x07) * 100 +
        (cls_number & 0xFF);
    ring("[%s:%u] Error %u",
         node->params.remote_addr,
         (unsigned)node->params.remote_port,
         (unsigned)node->status.map_error);
    return ES_EOK;
}

es_status
es_local_recv(es_node *node)
{
    char buf[4096];
    unsigned int addr_len;
    struct timeval timeout;
    struct sockaddr_in addr = {0};
    fd_set fds;
    int ret;
    stun_hdr *hdr;
    uint16_t message_type;
    es_msg msg;
    es_bool processed = ES_FALSE;

    msg.hdr = (stun_hdr *)buf;
    msg.max_len = sizeof(buf);

    FD_ZERO(&fds);
    FD_SET(node->sk, &fds);

    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = 2;

    ret = select(node->sk + 1, &fds, NULL, NULL, &timeout);
    if (ret == 0)
    {
        dbg("Selecting - no data");
        return ES_ENODATA;
    }
    else if (ret == -1)
    {
        dbg("Selecting - recv failed");
        return ES_ERECVFAIL;
    }

    dbg("Selecting");
    ret = recvfrom(node->sk, buf, sizeof(buf), 0, (struct sockaddr *)&addr,
                   &addr_len);
    if (ret == -1)
    {
        err("Receive failure");
        return ES_ERECVFAIL;
    }

    ES_BREAKABLE_START()
    {
        hdr = (stun_hdr *)buf;
        if (ret < sizeof(stun_hdr))
        {
            dbg("Not STUN response: %d vs %lu", ret, sizeof(stun_hdr));
            break;
        }

        if (ntohl(hdr->magic_cookie) != STUN_MAGIC_COOKIE)
        {
            dbg("Invalid STUN magic: %x vs %x", ntohl(hdr->magic_cookie),
                STUN_MAGIC_COOKIE);
            break;
        }

        if (memcmp(hdr->tid,
                   node->status.expected_tid,
                   sizeof(node->status.expected_tid)) != 0)
        {
            err("Unexpected transaction ID");
            return ES_EWRONGTID;
        }

        message_type = ntohs(hdr->message_type);
        switch (message_type)
        {
            case STUN_MSG_TYPE_BINDING_RESPONSE:
                return es_local_process_binding_response(node, &msg);
            case STUN_MSG_TYPE_BINDING_ERROR:
                return es_local_process_binding_error(node, &msg);
            default:
                warn("Got unsupported message");
                break;
        }

        processed = ES_TRUE;
    }
    ES_BREAKABLE_END();

    if (processed)
        return ES_EOK;

    /* can be connection request */
    return es_local_conn_request(node, buf, ret);
}