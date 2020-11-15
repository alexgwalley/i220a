#include "int_compare.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/** Return index of element in a[nElements]; < 0 if not found. */
int search_for_element(int a[], int nElements, int element);


/** Perform nTests searches for all elements in a[].  
 */
static void
do_search(int a[], int nElements, int nTests)
{
  for (int t = 0; t < nTests; t++) {
    for (int i = 0; i < nElements; i++) {
      int foundIndex = search_for_element(a, nElements, a[i]);
      assert(foundIndex == i);
      int foundIndex1 = search_for_element(a, nElements, a[i] + 1);
      assert(foundIndex1 < 0 || foundIndex1 == i + 1);
    }
  }
}

int 
main(int argc, const char *argv[])
{
  int nTests = -1;
  int nElements = -1;
  if (argc != 3 || (nElements = atoi(argv[1])) <= 0 ||
      (nTests = atoi(argv[2])) <= 0) {
    fprintf(stderr, "usage: %s NUM_ELEMENTS NUM_TESTS\n", argv[0]);
    exit(1);
  }
  int a[nElements];
  for (int i = 0; i < nElements; i++) {
    a[i] = rand();
  }
  qsort(a, nElements, sizeof(int), int_compare);
  do_search(a, nElements, nTests);
  return 0;
}
