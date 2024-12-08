#pragma once
#include "es_bool.h"

typedef enum es_status {
    ES_EOK,

    ES_ENOMEM,
    ES_ESTATE,
    ES_ENOTSUPP,
    ES_EIO,
    ES_EINVAL,
    ES_EFAIL,

    /* params */
    ES_EPARAMMISSING,
    ES_EPARAMINVALID,
    ES_EPARAMINSUFFICIENT,

    /* connection */
    ES_ECONNFAIL,
    ES_EDNSFAIL,
    ES_ESENDFAIL,
    ES_ESKCREATEFAIL,
    ES_ESKBINDFAIL,
    ES_ESKCLOEXECFAIL,
    ES_ESKNONBLOCKFAIL,

    ES_ENODATA,
    ES_ERECVFAIL,
    ES_ERESPONSEINVALID,

    ES_EWRONGTID,

    ES_ESCRIPTFAIL,
} es_status;

/** Check if connection is broken */
static es_bool es_status_is_conn_broken(es_status rc)
{
    switch (rc)
    {
        case ES_ERECVFAIL:
        case ES_ESENDFAIL:
            return ES_TRUE;
        default:
            return ES_FALSE;
    }
}
