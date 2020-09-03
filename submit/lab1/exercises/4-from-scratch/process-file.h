#ifndef _PROCESS_FILE_H_
#define _PROCESS_FILE_H

#include <stdio.h>

/** Process stream f (corresponding to fileName) for addition/lookup/removal
 *  of key=value pairs.
 */
void process_file(const char *fileName, FILE *f);

#endif //ifndef _PROCESS_FILE_H
