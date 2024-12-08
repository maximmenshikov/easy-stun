#pragma once


#define EXIT_ON_ERROR(__msg, __rc) do { \
    if ((__rc) != ES_EOK) { \
        err("%s", __msg); \
        rc = (__rc); \
        goto err; \
    } \
} while (0)

#define UNUSED(__x) do { (void)(__x); } while (0)

#define PAD(__val, __pad) ({ \
    int __v = (__pad) - ((__val) % (__pad)); \
    if (__v == (__pad)) \
        __v = 0; \
    __v; \
})
