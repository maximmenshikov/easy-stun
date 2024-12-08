#pragma once
#include <inttypes.h>
#include <unistd.h>
#include "stun.h"
#include "es_params.h"

typedef struct es_msg {
    size_t max_len;
    stun_hdr *hdr;
} es_msg;

void es_msg_hdr_init(es_msg *msg, uint16_t type);
es_status es_msg_add_attr(es_msg *msg, stun_attr_type type, void *value, uint32_t value_len);
es_status es_msg_add_integrity(es_msg *msg, const char *password);
es_status es_msg_read_attr(es_msg *msg, stun_attr_type type, stun_attr **attr);