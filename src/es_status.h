#pragma once

typedef enum es_status {
    ES_EOK,

    ES_ENOMEM,

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
    ES_ENOTSUPP,
} es_status;
