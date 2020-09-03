#include "key-value.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Limits.  Very low to facilitate testing.
//A production program should not have any such limits.
enum { 
  MAX_KEY_LEN = 4,
  MAX_TABLE_ENTRIES = 4,
};

typedef struct {
  char key[MAX_KEY_LEN + 1];
  int value;
} KeyValue;

//error messages
#define BAD_VALUE_ERROR		"bad value"
#define KEY_NOT_FOUND_ERROR	"unknown key"
#define KEY_TOO_LONG_ERROR	"key too long"
#define TOO_MANY_ENTRIES_ERROR 	"too many key-value pairs"

//will be initialized to all zeros
static KeyValue table[MAX_TABLE_ENTRIES];

/** Return value of key of length keyLen in *valueP.  Return NULL if ok, else
 *  return error message.
 */
char *
lookup_table(const char *key, int keyLen, int *valueP)
{
  if (keyLen > MAX_KEY_LEN) {
    return KEY_TOO_LONG_ERROR;
  }
  else {
    for (int i = 0; i < MAX_TABLE_ENTRIES; i++) {
      if (strncmp(key, &table[i].key[0], keyLen) == 0) {
	*valueP = table[i].value;
	return NULL;
      }
    }
    return KEY_NOT_FOUND_ERROR;
  }
}

/** Add value for key of length keyLen to table.  Return NULL if ok, else
 *  return error message.  *value should point to a string which spells an
 *  integer.
 */
char *
add_table(const char *key, int keyLen, const char *valueP)
{
  if (keyLen > MAX_KEY_LEN) {
    return KEY_TOO_LONG_ERROR;
  }
  char *endP;
  int value = (int)strtol(valueP, &endP, 10);
  if (*endP != '\0') {
    return BAD_VALUE_ERROR;
  }
  else {
    for (int i = 0; i < MAX_TABLE_ENTRIES; i++) {
      if (strlen(table[i].key) == 0) {
	strncpy(table[i].key, key, keyLen);
	table[i].value = value;
	return NULL;
      }
    }
    return TOO_MANY_ENTRIES_ERROR;
  }
}

/** Remove mapping for key of length keyLen from table.  Return NULL if
 *  everything ok, else return error message.  
 */
char *
remove_table(const char *key, int keyLen)
{
  if (keyLen > MAX_KEY_LEN) {
    return KEY_TOO_LONG_ERROR;
  }
  else {
    for (int i = 0; i < MAX_TABLE_ENTRIES; i++) {
      if (strncmp(key, table[i].key, keyLen) == 0) {
	table[i].key[0] = '\0'; //make entry empty
	return NULL;
      }
    }
    return KEY_NOT_FOUND_ERROR;
  }
}
