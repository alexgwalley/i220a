#include "bcd.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

char get_bcd_digit(Bcd num, int digitIndex){
	return (num >> digitIndex*4) & 0xf;
}

void set_bcd_digit(Bcd* num, int digitIndex, Bcd toSet){
	*num |= (toSet&0xf) << (digitIndex*4);
}

/** Return BCD encoding of binary (which has normal binary representation).
 *
 *  Examples: binary_to_bcd(0xc) => 0x12;
 *            binary_to_bcd(0xff) => 0x255
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd
binary_to_bcd(Binary value, BcdError *error)
{
  Bcd res = 0;
  int index = 0;

  while(value != 0){
	if(index >= MAX_BCD_DIGITS){
		if(error != NULL) *error = OVERFLOW_ERR;
		break;
	}
	char digit = value % 10;
	/* printf("binary_to_bcd_res: %" BCD_FORMAT_MODIFIER , res); */
	value /= 10;
	set_bcd_digit(&res, index, (Bcd)digit);
	index ++;
  }
  return res;
}

/** Return binary encoding of BCD value bcd.
 *
 *  Examples: bcd_to_binary(0x12) => 0xc;
 *            bcd_to_binary(0x255) => 0xff
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR if bcd contains
 *  a bad BCD digit.
 *  Cannot overflow since Binary can represent larger values than Bcd
 */
Binary
bcd_to_binary(Binary bcd, BcdError *error)
{
  /* printf("Start of bcd_to_binary\n"); */
  Binary res = 0; 
  long pow = 1;
  while(bcd > 0){
	char digit = bcd & 0xf;
	/* printf("bcd_to_binary_bcd: 0x%08" BCD_FORMAT_MODIFIER "x\n", bcd); */
	bcd >>= 4;
	if(digit > 0x9){
		/* printf("Error:Invalid digit!\n"); */
		if(error != NULL) *error = BAD_VALUE_ERR;
		break;
	}
 	res += digit*pow;
	pow *= 10;	
  }
  return res;
}

/** Return BCD encoding of decimal number corresponding to string s.
 *  Behavior undefined on overflow or if s contains a non-digit
 *  character.  Rougly equivalent to atoi().
 *
 *  If error is not NULL, sets *error to OVERFLOW_ERR if binary is too
 *  big for the Bcd type, otherwise *error is unchanged.
 */
Bcd
str_to_bcd(const char *s, const char **p, BcdError *error)
{
  Bcd res = 0;

  int index = 0;
  *p = s;

  while(**p != '\0'){
	/* Convert char to binary */
	char digit = **p - '0';
			
	/* Check if is valid digit */
	if(digit > 9) break;	
	if(index >= MAX_BCD_DIGITS){
		if(error != NULL) *error = OVERFLOW_ERR;
		break;
	}
	
	/* Set digit */
 	res <<= 4;	
	set_bcd_digit(&res, 0, (Bcd)digit);	

	(*p)++;
	index ++;
  }

  return res;
}

/** Convert bcd to a NUL-terminated string in buf[] without any
 *  non-significant leading zeros.  Never write more than bufSize
 *  characters into buf.  The return value is the number of characters
 *  written (excluding the NUL character used to terminate strings).
 *
 *  If error is not NULL, sets *error to BAD_VALUE_ERR is bcd contains
 *  a BCD digit which is greater than 9, OVERFLOW_ERR if bufSize bytes
 *  is less than BCD_BUF_SIZE, otherwise *error is unchanged.
 */
int
bcd_to_str(Bcd bcd, char buf[], size_t bufSize, BcdError *error)
{
  int numCharWritten = 0;
  if(bufSize < BCD_BUF_SIZE){
	if(error != NULL) *error = OVERFLOW_ERR;
	return numCharWritten;
  } 

  for (int i = 0; i < bufSize; i++) {
	if (get_bcd_digit(bcd, i) > 0x9) {
		if (error != NULL) *error = BAD_VALUE_ERR;
		return 0;
	}
  } 

  Binary bin = bcd_to_binary (bcd, error); 
  numCharWritten = snprintf(buf, bufSize, "%d" BCD_FORMAT_MODIFIER, bin);
  return numCharWritten;
}

/** Return the BCD representation of the sum of BCD int's x and y.
 *
 *  If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 *  contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 *  overflow, otherwise *error is unchanged.
 */
Bcd
bcd_add(Bcd x, Bcd y, BcdError *error)
{
  //@TODO
  Bcd sum = 0;
  Binary x_bin = bcd_to_binary(x, error);
  Binary y_bin = bcd_to_binary(y, error);
  sum = binary_to_bcd (x_bin + y_bin, error);
  return sum;
}

/** Return the BCD representation of the product of BCD int's x and y.
 *
 * If error is not NULL, sets *error to to BAD_VALUE_ERR is x or y
 * contains a BCD digit which is greater than 9, OVERFLOW_ERR on
 * overflow, otherwise *error is unchanged.
 */
Bcd
bcd_multiply(Bcd x, Bcd y, BcdError *error)
{
  //@TODO
  return 0;
}

