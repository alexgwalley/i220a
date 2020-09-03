#include "key-value.h"
#include "process-file.h"

#include <ctype.h>
#include <string.h>

static void 
process_line(const char *fileName, const char *line, int lineNum)
{
  int i = 0;
  while (isalnum(line[i])) {
    i++;
  }
  const char *msg;
  switch (line[i]) {
    case '=':
      msg = add_table(line, i, &line[i + 1]);
      break;
    case '?': {
      int value;
      if ((msg = lookup_table(line, i, &value)) == NULL) {
	printf("%d\n", value);
      }
      break;
    }
    case '-':
      msg = remove_table(line, i);
      break;
    default:
      msg = "bad input";
      break;
  }
  if (msg != NULL) {
    fprintf(stderr, "%s:%d: %s\n", fileName, lineNum, msg);
  }
}

void 
process_file(const char *fileName, FILE *f)
{
  enum { LINE_MAX = 10 };
  char line[LINE_MAX];
  int lineNum = 0;
  while (fgets(line, LINE_MAX, f) != NULL) {
    lineNum++;
    if (line[strlen(line) - 1] != '\n') {
      fprintf(stderr, "%s:%d: line too long", fileName, lineNum);
      continue;
    }
    line[strlen(line) - 1] = '\0'; //clobber '\n'
    process_line(fileName, line, lineNum);
  }
}
