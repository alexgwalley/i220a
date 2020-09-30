//fix for check not supporting long and long long.

#define _ck_assert_ulong(X, OP, Y) do { \
  unsigned long _ck_x = (X); \
  unsigned long _ck_y = (Y); \
  ck_assert_msg(_ck_x OP _ck_y, "Assertion '%s' failed: %s == %ju, %s == %ju", #X" "#OP" "#Y, #X, _ck_x, #Y, _ck_y); \
} while (0)
#define ck_assert_ulong_eq(X, Y) _ck_assert_ulong(X, ==, Y)
#define ck_assert_ulong_ne(X, Y) _ck_assert_ulong(X, !=, Y)
#define ck_assert_ulong_lt(X, Y) _ck_assert_ulong(X, <, Y)
#define ck_assert_ulong_gt(X, Y) _ck_assert_ulong(X, >, Y)
#define ck_assert_ulong_le(X, Y) _ck_assert_ulong(X, <=, Y)
#define ck_assert_ulong_ge(X, Y) _ck_assert_ulong(X, >=, Y)

#define _ck_assert_ulong_long(X, OP, Y) do { \
  unsigned long long _ck_x = (X); \
  unsigned long long _ck_y = (Y); \
  ck_assert_msg(_ck_x OP _ck_y, "Assertion '%s' failed: %s == %ju, %s == %ju", #X" "#OP" "#Y, #X, _ck_x, #Y, _ck_y); \
} while (0)
#define ck_assert_ulong_long_eq(X, Y) _ck_assert_ulong_long(X, ==, Y)
#define ck_assert_ulong_long_ne(X, Y) _ck_assert_ulong_long(X, !=, Y)
#define ck_assert_ulong_long_lt(X, Y) _ck_assert_ulong_long(X, <, Y)
#define ck_assert_ulong_long_gt(X, Y) _ck_assert_ulong_long(X, >, Y)
#define ck_assert_ulong_long_le(X, Y) _ck_assert_ulong_long(X, <=, Y)
#define ck_assert_ulong_long_ge(X, Y) _ck_assert_ulong_long(X, >=, Y)
