#ifndef _YAS_H
#define _YAS_H

#include "y86.h"

/** Assemble and load yasFiles into y86.  Return true iff no errors. */
bool yas_to_y86(Y86 *y86, int numFiles, const char *yasFiles[]);

#if 0
/** Write image of assembled code for yasFiles to text file hex
 *  in Intel HEX format.  Return true iff no errors.
 */
bool yas_to_hex(FILE *hex, int numFiles, const char *yasFiles[]);
#endif

/** If no errors, write listing of assembled code for yasFiles to text
 *  file listing.
 */
void yas_to_listing(FILE *listing, int numFiles, const char *yasFiles[]);

#endif //ifndef _YAS_H
