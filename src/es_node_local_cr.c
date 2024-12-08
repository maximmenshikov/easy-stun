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

es_status
es_local_conn_request(es_node *node, const char *buf, uint32_t buf_len)
{
    char full_cmd[1024];
    int ret;

    sprintf(full_cmd, "%s cr %s %u", node->params.script,
        node->status.mapped_addr,
        (unsigned)node->status.mapped_port);
    ret = system(full_cmd);
    ring("Script '%s' executed with return code %d", node->params.script, ret);
    if (ret != 0)
    {
        return ES_ESCRIPTFAIL;
    }

    return ES_EOK;
}
