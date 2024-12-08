#pragma once
#include <inttypes.h>

typedef enum stun_attr_type {
    STUN_ATTR_MAPPED_ADDRESS        = 0x0001,
    STUN_ATTR_USERNAME              = 0x0006,
    STUN_ATTR_MESSAGE_INTEGRITY     = 0x0008,
    STUN_ATTR_ERROR_CODE            = 0x0009,
    STUN_ATTR_UNKNOWN_ATTRIBUTES    = 0x000A,
    STUN_ATTR_REALM                 = 0x0014,
    STUN_ATTR_NONCE                 = 0x0015,
    STUN_ATTR_XOR_MAPPED_ADDRESS    = 0x0020,
    STUN_ATTR_CONN_REQUEST_BINDING  = 0xC001,
    STUN_ATTR_BINDING_CHANGE        = 0xC002,
} stun_attr_type;

typedef struct stun_attr {
    uint16_t type;
    uint16_t length;
    uint8_t  value[];
} stun_attr;

typedef struct stun_hdr
{
	uint16_t message_type;
	uint16_t message_len;
	uint32_t magic_cookie;
	uint8_t  tid[12];
	uint8_t  data[0];
} stun_hdr;
