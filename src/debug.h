#pragma once
#include <stdio.h>

#if DBG_LEVEL >= 4
#define dbg(__fmt...) do { \
    fprintf(stderr, "dbg: " __fmt); \
    fprintf(stderr, "\n"); \
} while (0)
#else
#define dbg(__fmt...) do { } while (0)
#endif

#if DBG_LEVEL >= 3
#define warn(__fmt...) do { \
    fprintf(stderr, "warn: " __fmt); \
    fprintf(stderr, "\n"); \
} while (0)
#else
#define warn(__fmt...) do { } while (0)
#endif

#if DBG_LEVEL >= 2
#define err(__fmt...) do { \
    fprintf(stderr, "err: " __fmt); \
    fprintf(stderr, "\n"); \
} while (0)
#else
#define err(__fmt...) do { } while (0)
#endif

#if DBG_LEVEL >= 1
#define ring(__fmt...) do { \
    fprintf(stderr, __fmt); \
    fprintf(stderr, "\n"); \
} while (0)
#else
#define ring(__fmt...) do { } while (0)
#endif

#define crit(__fmt...) ring(__fmt)
