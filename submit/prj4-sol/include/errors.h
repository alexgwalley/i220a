#ifndef _ERRORS_H
#define _ERRORS_H

#include <stdio.h>

/* Print a error message on stderr as per printf-style fmt and
 * optional arguments.  If fmt ends with ':', follow with
 * a space, strerror(errno), followed by a newline.
 */
__attribute__ ((format(printf, 1, 2)))
void error(const char *fmt, ...);

/* Print a error message on err as per printf-style fmt and
 * optional arguments.  If fmt ends with ':', follow with
 * a space, strerror(errno), followed by a newline.
 */
__attribute__ ((format(printf, 2, 3)))
void errorf(FILE *err, const char *fmt, ...);

/* Print a error message on stderr as per printf-style fmt and
 * optional arguments.  If fmt ends with ':', follow with a space,
 * strerror(errno), followed by a newline.  Terminate the program.
 */
__attribute__ ((noreturn,  format(printf, 1, 2)))
void fatal(const char *fmt, ...);

/* Print a error message on err as per printf-style fmt and
 * optional arguments.  If fmt ends with ':', follow with a space,
 * strerror(errno), followed by a newline.  Terminate the program.
 */
__attribute__ ((noreturn,  format(printf, 2, 3)))
void fatalf(FILE *err, const char *fmt, ...);

/* Return # of errors seen so far. */
int getErrorCount(void);

#endif /* #ifndef _ERRORS_H */
