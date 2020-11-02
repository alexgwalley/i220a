#include <stdio.h>
#include <stdlib.h>

/** External routine which returns cpuid info for op in *eax, *ebx,
 *  *ecx, *edx (corresponding to values returned in like-named
 *  registers by x86 cpuid instruction).
 */
void get_cpuid(int op, int *eax, int *ebx, int *ecx, int *edx);

/** Decode vendor string in ebx:edx:ecx into vendor_string (which
 *  is assumed to be large enough.
 */
static void
decode_vendor_string(int ebx, int ecx, int edx, char *vendor_string)
{
  *( ((int *)vendor_string) + 0 ) = ebx;
  *( ((int *)vendor_string) + 1 ) = edx;
  *( ((int *)vendor_string) + 2 ) = ecx;
  *(char *)(((int *)vendor_string + 3)) = '\0';
}

/** Output cpuid for op specified by first command-line argument on
 *  stdout. If op is 0, decode and output vendor info too.
 */
int
main(int argc, const char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "usage: %s CPUID_OP\n", argv[0]);
    exit(1);
  }
  int op = atoi(argv[1]);
  int eax, ebx, ecx, edx;
  get_cpuid(op, &eax, &ebx, &ecx, &edx);
  printf("cpuid(%d): eax = 0x%x; ebx = 0x%x; edx = 0x%x; ecx = 0x%x\n",
         op, eax, ebx, edx, ecx);
  if (op == 0) {
    char vendor_string[3*4 + 1];
    decode_vendor_string(ebx, ecx, edx, vendor_string);
    printf("vendor: %s\n", vendor_string);
  }
  return 0;
}
