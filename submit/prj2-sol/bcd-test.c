#include "bcd.h"

#include "check-extra.h"

#include <check.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if BCD_BASE == 3
#define ck_assert_bcd_eq ck_assert_ulong_eq
#elif BCD_BASE == 4
#define ck_assert_bcd_eq ck_assert_ulong_long_eq
#else
#define ck_assert_bcd_eq ck_assert_int_eq
#endif

typedef struct {
  const char *descr;
  Binary binary;
  Bcd bcd;
  char str[BCD_BUF_SIZE];
} BcdInfo;

typedef struct Data {
  BcdInfo zero;           //0
  BcdInfo consecutive;    //consecutive digits 123...89012...
  BcdInfo max;            //all 9's
  BcdInfo overflow;       //max + 1
  BcdInfo max4;           //max - 4
  BcdInfo maxHalfTrunc;   //truncated max/2: 499...99
  BcdInfo maxHalfRound;   //500...00
  BcdInfo badVal;         //9999...99a
} Data;

Data DATA;

static Binary
makeBinary(int digit) {
  Binary val = 0;
  int d = 1;
  for (int i = 0; i < MAX_BCD_DIGITS; i++) {
    val = val*10 + (digit < 0 ? d : digit);
    d = (d + 1)%10;
  }
  return val;
}

static void
fillBcdInfo(const char *descr, Binary binary, BcdInfo *info)
{
  info->descr = descr;
  info->binary = binary;
  sprintf(info->str, "%" BCD_FORMAT_MODIFIER "u", binary);
  sscanf(info->str, "%" SCANF_MODIFIER "x", &info->bcd);
}

__attribute__((unused))
static const char *
errStr(BcdError err) {
  switch (err) {
    case OK_ERR:
      return "OK_ERR";
    case BAD_VALUE_ERR:
      return "BAD_VALUE_ERR";
    case OVERFLOW_ERR:
      return "OVERFLOW_ERR";
    default:
      fprintf(stderr, "error value %d not handled\n", err);
      exit(1);
  }
  return NULL;
}

#if BCD_TEST_TRACE
#define BCD_TRACE(actual, expected, ...) \
  do { \
    fprintf(stderr, "  bcd value: "); \
    fprintf(stderr, "actual = 0x%" BCD_FORMAT_MODIFIER "x; ", actual);  \
    fprintf(stderr, "expected = 0x%" BCD_FORMAT_MODIFIER "x\n", expected); \
  } while (0)
#define CHAR_TRACE(actual, expected, ...) \
  do { \
    fprintf(stderr, "  pointer points to: "); \
    fprintf(stderr, "actual = '%c'; expected = '%c'\n", actual, expected); \
  } while (0)
#define ERR_TRACE(actual, expected, ...) \
  do { \
    fprintf(stderr, "  error: "); \
    fprintf(stderr, "actual = %s; ", errStr(actual)); \
    fprintf(stderr, "expected = %s\n", errStr(expected)); \
  } while (0)
#define INT_TRACE(actual, expected, ...) \
  do { \
    fprintf(stderr, "  int value: "); \
    fprintf(stderr, "actual = %d; expected = %d\n", \
            (int)actual, (int)expected); \
  } while (0)
#define STR_TRACE(actual, expected, ...) \
  do { \
    fprintf(stderr, "  bcd string: "); \
    fprintf(stderr, "actual = %s; expected = %s\n", actual, expected); \
  } while (0)
#define TEST_TRACE(...) \
  do { \
    fprintf(stderr,"TEST " __VA_ARGS__); \
    fprintf(stderr,"\n"); \
  } while (0)
#else
#define BCD_TRACE(...)
#define CHAR_TRACE(...)
#define ERR_TRACE(...)
#define INT_TRACE(...)
#define STR_TRACE(...)
#define TEST_TRACE(...)
#endif

__attribute__((unused))
static void
dumpTestData(const Data *const p, FILE *out)
{
  fprintf(out, "*** BEGIN_TEST DATA for BCD_BASE = %d\n", BCD_BASE);
  fprintf(out, "sizeof(Bcd) = %zu; BCD_MAX_DIGITS = %d\n",
          sizeof(Bcd), MAX_BCD_DIGITS);
  const BcdInfo *infos[] = {
    &p->zero, &p->consecutive, &p->max, &p->overflow, &p->max4,
    &p->maxHalfTrunc, &p->maxHalfRound, &p->badVal,
  };
  for (unsigned i = 0; i < sizeof(infos)/sizeof(infos[0]); i++) {
    const BcdInfo *infoP = infos[i];
    fprintf(out, "%s: ", infoP->descr);
    fprintf(out, "str: \"%s\": ", infoP->str);
    fprintf(out, "binary: %" BCD_FORMAT_MODIFIER "u; ", infoP->binary);
    fprintf(out, "bcd: 0x%" BCD_FORMAT_MODIFIER "x\n", infoP->bcd);
  }

  fprintf(out, "*** END_TEST DATA for BCD_BASE = %d\n", BCD_BASE);
}

static void
initTestData(Data *data)
{
  fillBcdInfo("zero", makeBinary(0), &DATA.zero);
  fillBcdInfo("consecutive", makeBinary(-1), &DATA.consecutive);
  fillBcdInfo("max", makeBinary(9), &DATA.max);
  fillBcdInfo("overflow", makeBinary(9) + 1, &DATA.overflow);
  fillBcdInfo("max - 4", makeBinary(9) - 4, &DATA.max4);
  fillBcdInfo("maxHalfTrunc", makeBinary(9)/2, &DATA.maxHalfTrunc);
  fillBcdInfo("maxHalfRound", makeBinary(9)/2 + 1, &DATA.maxHalfRound);
  fillBcdInfo("badVal", makeBinary(9), &DATA.badVal);
  DATA.badVal.bcd = (DATA.max.bcd & ~0xf) | 0xa;
  DATA.badVal.binary += 1;
  DATA.badVal.str[BCD_BUF_SIZE - 2] = 'a';

  #if DUMP_TEST_DATA
  dumpTestData(&DATA, stderr);
  #endif
}


/************************* binary_to_bcd() Tests ***********************/

START_TEST(binary_to_bcd_consecutive)
{
  TEST_TRACE("binary_to_bcd(\"%" BCD_FORMAT_MODIFIER "u\")",
             DATA.consecutive.binary);

  BcdError err = OK_ERR;
  Bcd result = binary_to_bcd(DATA.consecutive.binary, &err);

  BCD_TRACE(result, DATA.consecutive.bcd);
  ck_assert_bcd_eq(result, DATA.consecutive.bcd);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(binary_to_bcd_max)
{
  TEST_TRACE("binary_to_bcd(\"%" BCD_FORMAT_MODIFIER "u\")", DATA.max.binary);

  BcdError err = OK_ERR;
  Bcd result = binary_to_bcd(DATA.max.binary, &err);

  BCD_TRACE(result, DATA.max.bcd);
  ck_assert_bcd_eq(result, DATA.max.bcd);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(binary_to_bcd_max4)
{
  TEST_TRACE("binary_to_bcd(\"%" BCD_FORMAT_MODIFIER "u\")", DATA.max4.binary);

  BcdError err = OK_ERR;
  Bcd result = binary_to_bcd(DATA.max4.binary, &err);

  BCD_TRACE(result, DATA.max4.bcd);
  ck_assert_bcd_eq(result, DATA.max4.bcd);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(binary_to_bcd_maxHalfTrunc)
{
  TEST_TRACE("binary_to_bcd(\"%" BCD_FORMAT_MODIFIER "u\")",
             DATA.maxHalfTrunc.binary);

  BcdError err = OK_ERR;
  Bcd result = binary_to_bcd(DATA.maxHalfTrunc.binary, &err);

  BCD_TRACE(result, DATA.maxHalfTrunc.bcd);
  ck_assert_bcd_eq(result, DATA.maxHalfTrunc.bcd);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(binary_to_bcd_maxHalfRound)
{
  TEST_TRACE("binary_to_bcd(\"%" BCD_FORMAT_MODIFIER "u\")",
             DATA.maxHalfRound.binary);

  BcdError err = OK_ERR;
  Bcd result = binary_to_bcd(DATA.maxHalfRound.binary, &err);

  BCD_TRACE(result, DATA.maxHalfRound.bcd);
  ck_assert_bcd_eq(result, DATA.maxHalfRound.bcd);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST



START_TEST(binary_to_bcd_overflow)
{
  TEST_TRACE("binary_to_bcd(\"%" BCD_FORMAT_MODIFIER "u\"): overflow",
             DATA.overflow.binary);

  BcdError err = OK_ERR;

  binary_to_bcd(DATA.overflow.binary, &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST

__attribute__((unused))
static void
add_binary_to_bcd_tests(Suite *suite)
{
  TCase *binaryToBcd = tcase_create("binary_to_bcd");
  tcase_add_test(binaryToBcd, binary_to_bcd_consecutive);
  tcase_add_test(binaryToBcd, binary_to_bcd_overflow);
  tcase_add_test(binaryToBcd, binary_to_bcd_max);
  tcase_add_test(binaryToBcd, binary_to_bcd_max4);
  tcase_add_test(binaryToBcd, binary_to_bcd_maxHalfTrunc);
  tcase_add_test(binaryToBcd, binary_to_bcd_maxHalfRound);
  suite_add_tcase(suite, binaryToBcd);
}

/************************* bcd_to_binary() Tests ***********************/

START_TEST(bcd_to_binary_consecutive)
{
  TEST_TRACE("bcd_to_binary(\"0x%" BCD_FORMAT_MODIFIER "x\")",
             DATA.consecutive.bcd);

  BcdError err = OK_ERR;
  Binary result = bcd_to_binary(DATA.consecutive.bcd, &err);

  BCD_TRACE(result, DATA.consecutive.binary);
  ck_assert_bcd_eq(result, DATA.consecutive.binary);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_binary_max)
{
  TEST_TRACE("bcd_to_binary(\"0x%" BCD_FORMAT_MODIFIER "x\")", DATA.max.bcd);

  BcdError err = OK_ERR;
  Binary result = bcd_to_binary(DATA.max.bcd, &err);

  BCD_TRACE(result, DATA.max.binary);
  ck_assert_bcd_eq(result, DATA.max.binary);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_binary_max4)
{
  TEST_TRACE("bcd_to_binary(\"0x%" BCD_FORMAT_MODIFIER "x\")", DATA.max4.bcd);

  BcdError err = OK_ERR;
  Binary result = bcd_to_binary(DATA.max4.bcd, &err);

  BCD_TRACE(result, DATA.max4.binary);
  ck_assert_bcd_eq(result, DATA.max4.binary);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_binary_maxHalfTrunc)
{
  TEST_TRACE("bcd_to_binary(\"0x%" BCD_FORMAT_MODIFIER "x\")",
             DATA.maxHalfTrunc.bcd);

  BcdError err = OK_ERR;
  Binary result = bcd_to_binary(DATA.maxHalfTrunc.bcd, &err);

  BCD_TRACE(result, DATA.maxHalfTrunc.binary);
  ck_assert_bcd_eq(result, DATA.maxHalfTrunc.binary);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST


START_TEST(bcd_to_binary_maxHalfRound)
{
  TEST_TRACE("bcd_to_binary(\"0x%" BCD_FORMAT_MODIFIER "x\")",
             DATA.maxHalfRound.bcd);

  BcdError err = OK_ERR;
  Binary result = bcd_to_binary(DATA.maxHalfRound.bcd, &err);

  BCD_TRACE(result, DATA.maxHalfRound.binary);
  ck_assert_bcd_eq(result, DATA.maxHalfRound.binary);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_binary_badVal)
{
  TEST_TRACE("bcd_to_binary(\"0x%" BCD_FORMAT_MODIFIER "x\"): bad value",
             DATA.badVal.bcd);

  BcdError err = OK_ERR;
  bcd_to_binary(DATA.badVal.bcd, &err);

  ERR_TRACE(err, BAD_VALUE_ERR);
  ck_assert_int_eq(err, BAD_VALUE_ERR);
}
END_TEST

__attribute__((unused))
static void
add_bcd_to_binary_tests(Suite *suite)
{
  TCase *bcdToBinary = tcase_create("bcd_to_binary");
  tcase_add_test(bcdToBinary, bcd_to_binary_consecutive);
  tcase_add_test(bcdToBinary, bcd_to_binary_max);
  tcase_add_test(bcdToBinary, bcd_to_binary_max4);
  tcase_add_test(bcdToBinary, bcd_to_binary_maxHalfTrunc);
  tcase_add_test(bcdToBinary, bcd_to_binary_maxHalfRound);
  tcase_add_test(bcdToBinary, bcd_to_binary_badVal);
  suite_add_tcase(suite, bcdToBinary);
}

/************************** str_to_bcd() Tests *************************/

START_TEST(str_to_bcd_consecutive)
{
  TEST_TRACE("str_to_bcd(\"%s\")", DATA.consecutive.str);

  const char *p;
  BcdError err = OK_ERR;
  Bcd result = str_to_bcd(DATA.consecutive.str, &p, &err);

  BCD_TRACE(result, DATA.consecutive.bcd);
  ck_assert_bcd_eq(result, DATA.consecutive.bcd);

  CHAR_TRACE(*p, '\0');
  ck_assert_int_eq(*p, '\0');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(str_to_bcd_not_nul_terminator)
{
  char str[BCD_BUF_SIZE + 1];
  strcpy(str, DATA.consecutive.str);
  str[BCD_BUF_SIZE - 1] = 'X'; str[BCD_BUF_SIZE] = '\0';
  TEST_TRACE("str_to_bcd(\"%s\"): not NUL terminated", str);

  const char *p;
  BcdError err = OK_ERR;
  Bcd result = str_to_bcd(str, &p, &err);

  BCD_TRACE(result, DATA.consecutive.bcd);
  ck_assert_bcd_eq(result, DATA.consecutive.bcd);

  CHAR_TRACE(*p, 'X');
  ck_assert_int_eq(*p, 'X');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(str_to_bcd_max)
{
  TEST_TRACE("str_to_bcd(\"%s\")", DATA.max.str);

  const char *p;
  BcdError err = OK_ERR;
  Bcd result = str_to_bcd(DATA.max.str, &p, &err);

  BCD_TRACE(result, DATA.max.bcd);
  ck_assert_bcd_eq(result, DATA.max.bcd);

  CHAR_TRACE(*p, '\0');
  ck_assert_int_eq(*p, '\0');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(str_to_bcd_overflow)
{
  char str[BCD_BUF_SIZE + 1];
  str[0] = '1';
  strcpy(&str[1], DATA.max.str);
  TEST_TRACE("str_to_bcd(\"%s\"): overflow", str);

  const char *p;
  BcdError err = OK_ERR;
  str_to_bcd(str, &p, &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST


__attribute__((unused))
static void
add_str_to_bcd_tests(Suite *suite)
{
  TCase *strToBcd = tcase_create("str_to_bcd");
  tcase_add_test(strToBcd, str_to_bcd_consecutive);
  tcase_add_test(strToBcd, str_to_bcd_not_nul_terminator);
  tcase_add_test(strToBcd, str_to_bcd_max);
  tcase_add_test(strToBcd, str_to_bcd_overflow);
  suite_add_tcase(suite, strToBcd);
}

/************************** bcd_to_str() Tests *************************/

START_TEST(bcd_to_str_consecutive)
{

  TEST_TRACE("bcd_to_str(0x%" BCD_FORMAT_MODIFIER "x)", DATA.consecutive.bcd);

  BcdError err = OK_ERR;
  char str[BCD_BUF_SIZE];

  int n = bcd_to_str(DATA.consecutive.bcd, str, sizeof(str), &err);

  INT_TRACE(n, strlen(DATA.consecutive.str));
  ck_assert_int_eq(n, strlen(DATA.consecutive.str));

  STR_TRACE(str, DATA.consecutive.str);
  ck_assert_str_eq(str, DATA.consecutive.str);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_str_zero)
{

  TEST_TRACE("bcd_to_str(0x%" BCD_FORMAT_MODIFIER "x)", DATA.zero.bcd);

  BcdError err = OK_ERR;
  char str[BCD_BUF_SIZE];

  int n = bcd_to_str(DATA.zero.bcd, str, sizeof(str), &err);

  INT_TRACE(n, strlen(DATA.zero.str));
  ck_assert_int_eq(n, strlen(DATA.zero.str));

  STR_TRACE(str, DATA.zero.str);
  ck_assert_str_eq(str, DATA.zero.str);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_str_max)
{

  TEST_TRACE("bcd_to_str(0x%" BCD_FORMAT_MODIFIER "x)", DATA.max.bcd);

  BcdError err = OK_ERR;
  char str[BCD_BUF_SIZE];

  int n = bcd_to_str(DATA.max.bcd, str, sizeof(str), &err);

  INT_TRACE(n, strlen(DATA.max.str));
  ck_assert_int_eq(n, strlen(DATA.max.str));

  STR_TRACE(str, DATA.max.str);
  ck_assert_str_eq(str, DATA.max.str);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_to_str_overflow)
{

  TEST_TRACE("bcd_to_str(0x%" BCD_FORMAT_MODIFIER "x): overflow "
             "buffer too small", DATA.max.bcd);

  BcdError err = OK_ERR;
  char str[BCD_BUF_SIZE - 1];

  bcd_to_str(DATA.max.bcd, str, sizeof(str), &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST

START_TEST(bcd_to_str_bad_value)
{
  TEST_TRACE("bcd_to_str(0x%" BCD_FORMAT_MODIFIER "x) with bad value",
       DATA.badVal.bcd);

  BcdError err = OK_ERR;

  char str[BCD_BUF_SIZE];
  bcd_to_str(DATA.badVal.bcd, str, sizeof(str), &err);

  ERR_TRACE(err, BAD_VALUE_ERR);
  ck_assert_int_eq(err, BAD_VALUE_ERR);
}
END_TEST

__attribute__((unused))
static void
add_bcd_to_str_tests(Suite *suite)
{
  TCase *bcdToStr = tcase_create("bcd_to_str");
  tcase_add_test(bcdToStr, bcd_to_str_consecutive);
  tcase_add_test(bcdToStr, bcd_to_str_max);
  tcase_add_test(bcdToStr, bcd_to_str_zero);
  tcase_add_test(bcdToStr, bcd_to_str_overflow);
  tcase_add_test(bcdToStr, bcd_to_str_bad_value);
  suite_add_tcase(suite, bcdToStr);
}

/**************************** bcd_add() Tests **************************/

START_TEST(bcd_add_consecutive)
{
  TEST_TRACE("bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)",
             DATA.consecutive.bcd, (Bcd)0x1);

  BcdError err = OK_ERR;
  Bcd sum = bcd_add(DATA.consecutive.bcd, 0x1, &err);

  Bcd expected = binary_to_bcd(DATA.consecutive.binary + 1, NULL);
  BCD_TRACE(sum, expected);
  ck_assert_bcd_eq(sum, expected);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_add_carry)
{
  Bcd arg1 = DATA.maxHalfTrunc.bcd;
  Bcd arg2 = DATA.maxHalfRound.bcd;
  TEST_TRACE("bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)",
             arg1, arg2);

  BcdError err = OK_ERR;
  Bcd sum = bcd_add(arg1, arg2, &err);

  Bcd expected = DATA.max.bcd;
  BCD_TRACE(sum, expected);
  ck_assert_bcd_eq(sum, expected);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_add_bad_value1)
{
  TEST_TRACE("bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x): "
             "bad first arg", DATA.badVal.bcd, (Bcd)0x1);

  BcdError err = OK_ERR;
  bcd_add(DATA.badVal.bcd, 0x1, &err);

  ERR_TRACE(err, BAD_VALUE_ERR);
  ck_assert_int_eq(err, BAD_VALUE_ERR);
}
END_TEST

START_TEST(bcd_add_bad_value2)
{
  TEST_TRACE("bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x): "
             "bad second arg", (Bcd)0x1, DATA.badVal.bcd);

  BcdError err = OK_ERR;
  bcd_add(0x1, DATA.badVal.bcd, &err);

  ERR_TRACE(err, BAD_VALUE_ERR);
  ck_assert_int_eq(err, BAD_VALUE_ERR);
}
END_TEST


START_TEST(bcd_add_overflow1)
{
  TEST_TRACE("bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x): "
             "overflow", DATA.max.bcd, (Bcd)0x1);

  BcdError err = OK_ERR;
  bcd_add(DATA.max.bcd, 0x1, &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST

START_TEST(bcd_add_overflow2)
{
  TEST_TRACE("bcd_add(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x): "
             "overflow", (Bcd)0x5, DATA.max4.bcd);

  BcdError err = OK_ERR;
  bcd_add(0x5, DATA.max4.bcd, &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST

__attribute__((unused))
static void
add_bcd_add_tests(Suite *suite)
{
  TCase *bcdAdd = tcase_create("bcd_add");
  tcase_add_test(bcdAdd, bcd_add_consecutive);
  tcase_add_test(bcdAdd, bcd_add_carry);
  tcase_add_test(bcdAdd, bcd_add_bad_value1);
  tcase_add_test(bcdAdd, bcd_add_bad_value2);
  tcase_add_test(bcdAdd, bcd_add_overflow1);
  tcase_add_test(bcdAdd, bcd_add_overflow2);
  suite_add_tcase(suite, bcdAdd);
}

/************************** bcd_multiply() Tests ***********************/

START_TEST(bcd_multiply_consecutive)
{
  TEST_TRACE("bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)",
             DATA.consecutive.bcd, (Bcd)0x2);

  BcdError err = OK_ERR;
  Bcd product = bcd_multiply(DATA.consecutive.bcd, 0x2, &err);

  Bcd expected = binary_to_bcd(DATA.consecutive.binary*2, NULL);
  BCD_TRACE(product, expected);
  ck_assert_bcd_eq(product, expected);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_multiply_carry)
{
  TEST_TRACE("bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)",
             (Bcd)0x9, (Bcd)0x9);

  BcdError err = OK_ERR;
  Bcd product = bcd_multiply(0x9, 0x9, &err);

  BCD_TRACE(product, (Bcd)0x81);
  ck_assert_bcd_eq(product, 0x81);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_multiply_bad_value1)
{
  TEST_TRACE("bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)"
             ": bad first arg", DATA.badVal.bcd, (Bcd)0x1);

  BcdError err = OK_ERR;
  bcd_multiply(DATA.badVal.bcd, 0x1, &err);

  ERR_TRACE(err, BAD_VALUE_ERR);
  ck_assert_int_eq(err, BAD_VALUE_ERR);
}
END_TEST

START_TEST(bcd_multiply_bad_value2)
{
  TEST_TRACE("bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)"
             ": bad second arg", (Bcd)0x1, DATA.badVal.bcd);

  BcdError err = OK_ERR;
  bcd_multiply(0x1, DATA.badVal.bcd, &err);

  ERR_TRACE(err, BAD_VALUE_ERR);
  ck_assert_int_eq(err, BAD_VALUE_ERR);
}
END_TEST

START_TEST(bcd_multiply_no_overflow)
{
  TEST_TRACE("bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)",
             DATA.maxHalfTrunc.bcd, (Bcd)0x2);


  BcdError err = OK_ERR;
  Bcd product = bcd_multiply(DATA.maxHalfTrunc.bcd, 0x2, &err);

  Bcd expected = binary_to_bcd(DATA.maxHalfTrunc.binary*0x2, NULL);
  BCD_TRACE(product, expected);
  ck_assert_bcd_eq(product, expected);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(bcd_multiply_overflow)
{
  TEST_TRACE("bcd_multiply(0x%" BCD_FORMAT_MODIFIER "x, 0x%"
             BCD_FORMAT_MODIFIER "x)",
             DATA.maxHalfRound.bcd, (Bcd)0x2);

  BcdError err = OK_ERR;
  bcd_multiply(DATA.maxHalfRound.bcd, 0x2, &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST

__attribute__((unused))
static void
add_bcd_multiply_tests(Suite *suite)
{
  TCase *bcdMultiply = tcase_create("bcd_multiply");
  tcase_add_test(bcdMultiply, bcd_multiply_consecutive);
  tcase_add_test(bcdMultiply, bcd_multiply_carry);
  tcase_add_test(bcdMultiply, bcd_multiply_bad_value1);
  tcase_add_test(bcdMultiply, bcd_multiply_bad_value2);
  tcase_add_test(bcdMultiply, bcd_multiply_no_overflow);
  tcase_add_test(bcdMultiply, bcd_multiply_overflow);
  suite_add_tcase(suite, bcdMultiply);
}

/***************************** Multiop Tests ***************************/

START_TEST(multiop_expr1)
{
  //39a * 2 + 3Z == 81
  TEST_TRACE("multi_op: \"39a\" * \"2\" + \"3Z\"");

  BcdError err = OK_ERR;
  const char *p;
  Bcd op1 = str_to_bcd("39a", &p, &err);

  CHAR_TRACE(*p, 'a');
  ck_assert_int_eq(*p, 'a');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd op2 = str_to_bcd("2", &p, &err);

  CHAR_TRACE(*p, '\0');
  ck_assert_int_eq(*p, '\0');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd op3 = str_to_bcd("3Z", &p, &err);

  CHAR_TRACE(*p, 'Z');
  ck_assert_int_eq(*p, 'Z');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd product = bcd_multiply(op1, op2, &err);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd sum = bcd_add(product, op3, &err);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  char result[BCD_BUF_SIZE];
  bcd_to_str(sum, result, sizeof(result), &err);

  STR_TRACE(result, "81");
  ck_assert_str_eq(result, "81");

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(multiop_expr2)
{
  //(29a + 2) * 3Z == 93
  TEST_TRACE("multi_op: (\"29a\" + \"2\") * \"3Z\"");

  BcdError err = OK_ERR;
  const char *p;
  Bcd op1 = str_to_bcd("29a", &p, &err);

  CHAR_TRACE(*p, 'a');
  ck_assert_int_eq(*p, 'a');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd op2 = str_to_bcd("2", &p, &err);

  CHAR_TRACE(*p, '\0');
  ck_assert_int_eq(*p, '\0');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd op3 = str_to_bcd("3Z", &p, &err);

  CHAR_TRACE(*p, 'Z');
  ck_assert_int_eq(*p, 'Z');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd sum = bcd_add(op1, op2, &err);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  Bcd product = bcd_multiply(sum, op3, &err);

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  char result[BCD_BUF_SIZE];
  bcd_to_str(product, result, sizeof(result), &err);

  STR_TRACE(result, "93");
  ck_assert_str_eq(result, "93");

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
}
END_TEST

START_TEST(multiop_overflow)
{
  TEST_TRACE("multi_op_overflow: \"%s\" * \"2\"", DATA.maxHalfRound.str);
  BcdError err = OK_ERR;
  const char *p;
  Bcd op1 = str_to_bcd(DATA.maxHalfRound.str, &p, &err);

  CHAR_TRACE(*p, '\0');
  ck_assert_int_eq(*p, '\0');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);
  Bcd op2 = str_to_bcd("2", &p, &err);

  CHAR_TRACE(*p, '\0');
  ck_assert_int_eq(*p, '\0');

  ERR_TRACE(err, OK_ERR);
  ck_assert_int_eq(err, OK_ERR);

  bcd_multiply(op1, op2, &err);

  ERR_TRACE(err, OVERFLOW_ERR);
  ck_assert_int_eq(err, OVERFLOW_ERR);
}
END_TEST

__attribute__((unused))
static void
add_bcd_multiop_tests(Suite *suite)
{
  TCase *multiop = tcase_create("multiop");
  tcase_add_test(multiop, multiop_expr1);
  tcase_add_test(multiop, multiop_expr2);
  tcase_add_test(multiop, multiop_overflow);
  suite_add_tcase(suite, multiop);

}
/*********************** Test Suite and Runner *************************/

#define binary_to_bcd_test 0x1
#define bcd_to_binary_test 0x2
#define str_to_bcd_test 0x4
#define bcd_to_str_test 0x8
#define bcd_add_test 0x10
#define bcd_multiply_test 0x20
#define bcd_multiop_test 0x40

#if TEST == 0
#undef TEST
#define TEST \
  (binary_to_bcd_test | bcd_to_binary_test | \
   str_to_bcd_test | bcd_to_str_test | \
   bcd_add_test | bcd_multiply_test | bcd_multiop_test )
#endif

static Suite *
bcd_suite(const char *name)
{
  Suite *suite = suite_create(name);

  #if TEST & binary_to_bcd_test
  add_binary_to_bcd_tests(suite);
  #endif
  #if TEST & bcd_to_binary_test
  add_bcd_to_binary_tests(suite);
  #endif
  #if TEST & str_to_bcd_test
  add_str_to_bcd_tests(suite);
  #endif
  #if TEST & bcd_to_str_test
  add_bcd_to_str_tests(suite);
  #endif
  #if TEST & bcd_add_test
  add_bcd_add_tests(suite);
  #endif
  #if TEST & bcd_multiply_test
  add_bcd_multiply_tests(suite);
  #endif
  #if TEST & bcd_multiop_test
  add_bcd_multiop_tests(suite);
  #endif

  return suite;
}

int
main(void)
{
  #if BCD_TEST_TRACE
  fprintf(stderr, "*** BEGIN TESTS for BCD_BASE == %d\n", BCD_BASE);
  #endif

  initTestData(&DATA);
  char name[16];
  sprintf(name, "bcd-%d", BCD_BASE);
  Suite *suite = bcd_suite(name);
  SRunner *runner = srunner_create(suite);
  srunner_run_all(runner, CK_NORMAL);
  int nFail = srunner_ntests_failed(runner);
  srunner_free(runner);

  #if BCD_TEST_TRACE
  fprintf(stderr, "*** END TESTS for BCD_BASE == %d\n", BCD_BASE);
  #endif
  return nFail != 0;
}
