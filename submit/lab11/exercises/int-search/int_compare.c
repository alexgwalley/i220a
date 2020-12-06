/** Return < 0, 0, > 0 when int pointed to by p1 is <, ==, > int pointed
 *  to by p2.
 */
int 
int_compare(const void *p1, const void *p2) {
  return *(int*)p1 - *(int*)p2;
}

