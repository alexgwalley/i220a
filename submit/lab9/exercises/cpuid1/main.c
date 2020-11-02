#include <stdio.h>
#include <stdlib.h>

/** External routine which returns cpuid info in *eax, *ebx, *ecx, *edx
 *  (corresponding to values returned in like-named registers by x86
 *  cpuid instruction).
 */
void get_cpuid(int *eax, int *ebx, int *ecx, int *edx);

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

/** Output cpuid for cpuid-op set to 0 on stdout. Decode and output
 *  vendor info too.
 */
int
main(int argc, const char *argv[])
{
  int eax, ebx, edx, ecx;
  get_cpuid(&eax, &ebx, &edx, &ecx);
  printf("cpuid: eax = 0x%x; ebx = 0x%x; edx = 0x%x; ecx = 0x%x\n",
         eax, ebx, edx, ecx);
  char vendor_string[3*4 + 1];
  decode_vendor_string(ebx, ecx, edx, vendor_string);
  printf("vendor: %s\n", vendor_string);
  return 0;
}
