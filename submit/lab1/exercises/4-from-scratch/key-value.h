#ifndef _KEY_VALUE_H
#define _KEY_VALUE_H

/** Return value of key of length keyLen in *valueP.  Return NULL if ok, else
 *  return error message.
 */
char *lookup_table(const char *key, int keyLen, int *valueP);

/** Add value for key of length keyLen to table.  Return NULL if ok, else
 *  return error message.  *value should point to a string which spells an
 *  integer.
 */
char *add_table(const char *key, int keyLen, const char *value);

/** Remove mapping for key of length keyLen from table.  Return NULL if
 *  everything ok, else return error message.  
 */
char *remove_table(const char *key, int keyLen);

#endif //#ifndef _KEY_VALUE_H
