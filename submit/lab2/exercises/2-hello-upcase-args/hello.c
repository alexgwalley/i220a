#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Upcase the initial letter of each word from in, copying the
 *  results into out.  The caller is responsible to ensure that out
 *  has enough space for the strlen(in) characters of in.
 */
void
upcase_words(const char in[], char out[])
{
  int inWord = 0;
  int n = strlen(in);
  for (int i = 0; i < n; i++) {
    if (!inWord && isalpha(in[i])) {
      out[i] = toupper(in[i]);
    }
    else {
      out[i] = in[i];
    }
    inWord = isalpha(in[i]);
  }
}

static void
say_hello(const char *name)
{
  printf("Hello %s\n", name);
}

int
main(int argc, const char *argv[])
{
  if (argc == 1) {
    fprintf(stderr, "usage: %s NAME...\n", argv[0]);
    exit(1);
  }
  else {
    for (int i = 1; i < argc; i++) {
      const char *name = argv[i];
      char friendlyName[strlen(name)];
      upcase_words(name, friendlyName);
      say_hello(friendlyName);
    }
  }
  return 0;
}
