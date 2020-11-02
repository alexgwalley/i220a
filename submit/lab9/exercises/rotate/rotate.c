#include <limits.h>

inline unsigned
rotl(unsigned x, int shift)
{
  return (x << shift) | (x >> (sizeof(x)*CHAR_BIT - shift));
}

inline unsigned
rotr(unsigned x, int shift)
{
  return (x >> shift) | (x << (sizeof(x)*CHAR_BIT - shift));
}

unsigned
f(unsigned a, unsigned b, unsigned n)
{
  return rotl(a, n) + rotr(b, n);
}
