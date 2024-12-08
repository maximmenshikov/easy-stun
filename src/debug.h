#pragma once

#define DBG_LEVEL 3

#if DBG_LEVEL >= 3
#define dbg(__fmt, vars...) do { fprintf(stderr, "dbg: " __fmt "\n", vars); } while (0)
#else
#define dbg(__fmt, vars...) do { } while (0)
#endif

#if DBG_LEVEL >= 2
#define warn(__fmt, vars...) do { fprintf(stderr, "warn: " __fmt "\n", vars); } while (0)
#else
#define warn(__fmt, vars...) do { } while (0)
#endif


#if DBG_LEVEL >= 1
#define err(__fmt, vars...) do { fprintf(stderr, "err: " __fmt "\n", vars); } while (0)
#else
#define err(__fmt, vars...) do { } while (0)
#endif
