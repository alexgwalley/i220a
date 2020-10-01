#ifndef BCD_H_
#define BCD_H_

#include <limits.h>
#include <stddef.h>


/** Representation used for a BCD number: define based on BCD_BASE */
#ifndef BCD_BASE
  #define BCD_BASE 2
#endif

//Depending on BCD_BASE, conditionally define:
//  Bcd:                 C type used to represent a BCD
//  BCD_FORMAT_MODIFIER: modifier used to printf() a BCD
//  SCANF_MODIFIER:      modifier used to scanf() a BCD
#if BCD_BASE == 0
  typedef unsigned char Bcd;
  #define BCD_FORMAT_MODIFIER ""
  #define SCANF_MODIFIER "hh"
#elif BCD_BASE == 1
  typedef unsigned short Bcd;
  #define BCD_FORMAT_MODIFIER ""
  #define SCANF_MODIFIER "h"
#elif BCD_BASE == 2
  typedef unsigned Bcd;
  #define BCD_FORMAT_MODIFIER ""
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#elif BCD_BASE == 3
  typedef unsigned long Bcd;
  #define BCD_FORMAT_MODIFIER "l"
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#elif BCD_BASE == 4
  typedef unsigned long long Bcd;
  #define BCD_FORMAT_MODIFIER "ll"
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#else
  #error "invalid BCD_BASE value"
  //avoid subsequent compiler errors by providing definitions
  typedef unsigned long long Bcd;
  #define BCD_FORMAT_MODIFIER "ll"
  #define SCANF_MODIFIER BCD_FORMAT_MODIFIER
#endif

//The above modifiers can be used to portably print and read Bcd values
//(note that in C, adjacent string literals are concatenated together).
//
// Bcd bcd;
// printf("0x%" BCD_FORMAT_MODIFIER "x", bcd); //output in hex
// printf("%" BCD_FORMAT_MODIFIER "u", bcd);   //output in decimal
// scanf("%" SCANF_MODIFIER "x", &bcd);        //read from stdin to bcd

//use same C-type as Bcd to represent binary representation of a Bcd number
typedef Bcd Binary;

enum {
  //# of bits in a BCD digit
  BCD_BITS = 4,

  //max # of digits within a Bcd
  MAX_BCD_DIGITS = sizeof(Bcd) * CHAR_BIT / BCD_BITS,

  //buffer size (including '\0') needed for a string representation of a Bcd
  BCD_BUF_SIZE = MAX_BCD_DIGITS + 1
};

//error codes returned by following API
typedef enum {
  OK_ERR,                  //no error
  BAD_VALUE_ERR,           //binary BCD value contains a digit > 9
  OVERFLOW_ERR             //an overflow was detected
} BcdError;



/** Return BCD encoding of binary (which has normal binary representation).
 *
 *  Examples: binary_to_bcd(0xc) => 0x12;
 *            binary_to_bcd(0xff) => 0x255
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd binary_to_bcd(Binary value, BcdError *error);

/** Return binary encoding of BCD value bcd.
 *
 *  Examples: bcd_to_binary(0x12) => 0xc;
 *            bcd_to_binary(0x255) => 0xff
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR if bcd contains
 *  a bad BCD digit.
 *  Cannot overflow since Binary can represent larger values than Bcd
 */
Binary bcd_to_binary(Binary bcd, BcdError *error);

/** Return BCD encoding of decimal number corresponding to string s.
 *  Behavior undefined on overflow or if s contains a non-digit
 *  character.  Rougly equivalent to atoi().
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd str_to_bcd(const char *s, const char **p, BcdError *error);

/** Convert bcd to a NUL-terminated string in buf[] without any
 *  non-significant leading zeros.  Never write more than bufSize
 *  characters into buf.  The return value is the number of characters
 *  written (excluding the NUL character used to terminate strings).
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR is bcd contains
 *  a BCD digit which is greater than 9, OVERFLOW_ERR if bufSize bytes
 *  is less than BCD_BUF_SIZE, otherwise *error is unchanged.
 */
int bcd_to_str(Bcd bcd, char buf[], size_t bufSize, BcdError *error);

/** Return the BCD representation of the sum of BCD int's x and y.
 *
 *  If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 *  contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 *  overflow, otherwise *error is unchanged.
 */
Bcd bcd_add(Bcd x, Bcd y, BcdError *error);

/** Return the BCD representation of the product of BCD int's x and y.
 *
 * If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 * contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 * overflow, otherwise *error is unchanged.
 */
Bcd bcd_multiply(Bcd x, Bcd y, BcdError *error);


char get_bcd_digit(Bcd num, int digitIndex);
void set_bcd_digit(Bcd* num, int digitIndex, char toSet);

#endif //ifndef BCD_H_
