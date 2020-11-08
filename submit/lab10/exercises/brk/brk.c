#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROMPT ">> "

static void
how_to(void) {
  printf(
    "This program illustrates the how the upper limit of a program's data\n"
    "area (the \"brk\") changes as memory is allocated.  To interact with\n"
    "the program, type one of the following commands at the '" PROMPT
    "' prompt:\n\n"
    "To allocate a block, type a line consisting of a + followed by the\n"
    "size of the memory to be allocated.  The program will print out\n"
    "a non-negative integer giving an id for that memory block, the address\n"
    "at which the memory is allocated, the current program brk and the\n"
    "change in the size of the brk since the program started up.\n\n"
    "To free memory, type a - followed by the id of a currently allocated\n"
    "memory block.  The program will respond with the current program brk\n"
    "and the change in the size of the brk since the program started up.\n\n"
         );
}

static void
init_addresses(void **addr, int n)
{
  for (int i = 0; i < n; i++) addr[i] = NULL;
}

static int
next_id(void **addr, int n)
{
  for (int i = 0; i < n; i++) {
    if (addr[i] == NULL) return i;
  }
  return -1;
}

static void
do_malloc(int size, void **addr, int n, char *initBrk)
{
  int id = next_id(addr, n);
  if (id < 0) {
    fprintf(stderr, "no more memory segments, use - command to free\n");
  }
  else {
    if (!(addr[id] = malloc(size))) {
      fprintf(stderr, "memory allocation error: %s\n", strerror(errno));
    }
    printf("block[%d] allocated %p: brk = %p, "
           "brk change since program start = %zu\n",
           id, addr[id], sbrk(0), ((char *)sbrk(0)) - initBrk);
  }
}

static void
do_free(int id, void **addr, int n, char *initBrk)
{
  if (id < 0 || id >= n || addr[id] == NULL) {
    fprintf(stderr, "bad id %d: must be id in [0, %d] of currently "
            "allocated memory block\n", id, n - 1);
  }
  else {
    free(addr[id]);
    addr[id] = NULL;
    printf("block[%d] free'd; brk = %p, brk change since program start = %zu\n",
           id, sbrk(0), ((char *)sbrk(0)) - initBrk);
  }
}

static void
interact(FILE *in, int n)
{
  void *addresses[n];
  init_addresses(addresses, n);
  char *initBrk = sbrk(0);
  while (!feof(in) && printf(PROMPT) && fflush(stdout) == 0) {
    char c;
    if (fscanf(in, " %c", &c) != 1) continue;
    switch (c) {
      case '+': {
        int size = -1;
        if (fscanf(in, "%d", &size) != 1 || size <= 0) {
          fprintf(stderr, "bad input: type \"+ MEM_SIZE\"\n");
        }
        do_malloc(size, addresses, n, initBrk);
        break;
      }
      case '-': {
        int id = -1;
        if (fscanf(in, "%d", &id) != 1 || id < 0) {
          fprintf(stderr, "bad input: type \"- ID\"\n");
        }
        do_free(id, addresses, n, initBrk);
        break;
      }
      default: {
        fprintf(stderr, "bad input %c: type a line \"+ MEM_SIZE\""
                " or \"- ID\"\n", c);
        int ch;
        do { ch = fgetc(in); } while (ch != EOF && ch != '\n');
      }
    } //switch
  } //while (!feof(in))
}

extern char etext, edata, end; // The symbols must have some type,

static void
out_segment_ends(void)
{
  printf("First address past:\n");
  printf("    program text (etext)      %10p\n", &etext);
  printf("    initialized data (edata)  %10p\n", &edata);
  printf("    uninitialized data (end)  %10p\n", &end);
  printf("    brk (sbrk(0))             %10p\n\n", sbrk(0));
}

int
main(int argc, const char **argv)
{
  int n;
  if (argc != 2 || (n = atoi(argv[1])) <= 0) {
    fprintf(stderr, "usage: %s MAX_DYNAMIC_ALLOCS\n", argv[0]);
    exit(1);
  }
  how_to();
  out_segment_ends();
  interact(stdin, n);
  return 0;
}
