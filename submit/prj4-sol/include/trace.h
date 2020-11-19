#undef TRACE
#ifdef DO_TRACE

#include <stdio.h>
//first arg must be a literal string, not a var
#define TRACE(fmt, ...) do {                \
  fprintf(stderr, fmt "\n", ##__VA_ARGS__); \
} while (0)

#else
#define TRACE(...)
#endif
