#include "int_compare.h"

#include <stdlib.h>


/** Return index of element in a[nElements]; < 0 if not found. */
int 
search_for_element(int a[], int nElements, int element)
{
  int *p = bsearch(&element, a, nElements, sizeof(a[0]), int_compare);
  return (p) ? p - a : -1;		   
}
