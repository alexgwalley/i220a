#include "bcd.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static inline const char *
skip_whitespace(const char *p) {
  while (isspace(*p)) p++;
  return p;
}

static int
is_error(BcdError err)
{
  switch (err) {
    case BAD_VALUE_ERR:
      fprintf(stderr, "bad BCD value > 9\n");
      return 1;
    case OVERFLOW_ERR:
      fprintf(stderr, "BCD overflow\n");
      return 1;
    default:
      break;
  }
  return 0;
}

int
main()
{
  enum { LINE_MAX = 80 };
  char line[LINE_MAX];
  printf("BCD_BASE == %d, sizeof(Bcd) == %zu\n", BCD_BASE, sizeof(Bcd));
  while (printf(">> ") && fflush(stdout) == 0 &&
         fgets(line, LINE_MAX, stdin) != NULL) {
    BcdError err = OK_ERR;
    if (line[strlen(line) - 1] != '\n') {
      fprintf(stderr, "line too long ... ignored\n");
      continue;
    }
    const char *p = skip_whitespace(line);
    Bcd result = str_to_bcd(p, &p, &err);
    if (is_error(err)) continue;
    p = skip_whitespace(p);
    if (*p == '+') {
      p = skip_whitespace(p + 1);
      Bcd operand2 = str_to_bcd(p, &p, &err);
      if (is_error(err)) continue;
      result = bcd_add(result, operand2, &err);
      if (is_error(err)) continue;
    }
    else if (*p == '*') {
      p = skip_whitespace(p + 1);
      Bcd operand2 = str_to_bcd(p, &p, &err);
      if (is_error(err)) continue;
      result = bcd_multiply(result, operand2, &err);
      if (is_error(err)) continue;
    }
    p = skip_whitespace(p);
    if (*p != '\0') {
      fprintf(stderr, "bad input %s", line);
    }
    else {
      char buf[BCD_BUF_SIZE];
      bcd_to_str(result, buf, BCD_BUF_SIZE, &err);
      if (is_error(err)) continue;
      printf("%s (%" BCD_FORMAT_MODIFIER "u)\n", buf, result);
    }
  }
  if (ferror(stdin)) {
    fprintf(stderr, "i/o error on stdin: %s\n", strerror(errno));
  }
  return 0;
}
