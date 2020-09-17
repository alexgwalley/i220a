#include "hamming.h"

#include <assert.h>

/**
  All bitIndex'es are numbered starting at the LSB which is given index 1

  ** denotes exponentiation; note that 2**n == (1 << n)
*/

/** Return bit at bitIndex from word. */
static inline unsigned
get_bit(HammingWord word, int bitIndex)
{
  assert(bitIndex > 0);
  return (word >> (bitIndex-1))&1;
}

/** Return word with bit at bitIndex in word set to bitValue. */
static inline HammingWord
set_bit(HammingWord word, int bitIndex, unsigned bitValue)
{
  assert(bitIndex > 0);
  assert(bitValue == 0 || bitValue == 1);
  return (bitValue) ? word | (1<<(bitIndex-1)) : word & ~(1<<(bitIndex-1));
}

/** Given a Hamming code with nParityBits, return 2**nParityBits - 1,
 *  i.e. the max # of bits in an encoded word (# data bits + # parity
 *  bits).
 */
static inline unsigned
get_n_encoded_bits(unsigned nParityBits)
{
  return (2<<nParityBits) - 1;
}

/** Return non-zero if bitIndex indexes a bit which will be used for a
 *  Hamming parity bit; i.e. the bit representation of bitIndex
 *  contains only a single 1.
 */
static inline int
is_parity_position(int bitIndex)
{
  assert(bitIndex > 0);
  int num_bits_set = 0;
  int n = bitIndex;
  while(n){
	n &= (n-1);
	num_bits_set++;
  }
  return (num_bits_set == 1);
}

/** Return the parity over the data bits in word specified by the
 *  parity bit bitIndex.  The word contains a total of nBits bits.
 *  Equivalently, return parity over all data bits whose bit-index has
 *  a 1 in the same position as in bitIndex.
 */
static int
compute_parity(HammingWord word, int bitIndex, unsigned nBits)
{
  assert(bitIndex > 0);
  char res = -1;
  // XOR all of bits that contain a 1 at bitIndex
  for(int i = 1; i <= nBits; i++){
	if(i & bitIndex && i != bitIndex){
		if(res == -1) res = (word >> (i-1)) & 1;
		else res ^= (word >> (i-1)) & 1;
	}
	
  }
  return res;
}

/** Encode data using nParityBits Hamming code parity bits.
 *  Assumes data is within range of values which can be encoded using
 *  nParityBits.
 */
HammingWord
hamming_encode(HammingWord data, unsigned nParityBits)
{
  int numDataBits = (2<<nParityBits) - 1 - nParityBits;
  assert(data > 0);
  assert(data < (2<<numDataBits)-1); 

  // number of total bits is 2^m -1, m = nParityBits
  int numBits = (2<<nParityBits) - 1;
  HammingWord res = 0;


  for(int i = 1; i <= numBits; i++){
	if(!is_parity_position(i)) {
		res = set_bit(res, i, data&1);
		data >>= 1;
	}
  }

  int pos = 1;
  while(pos < numBits){
	res = set_bit(res, pos, compute_parity(res, pos, numBits));	
	pos <<= 1;
  }
	
  return res;
}

/** Decode encoded using nParityBits Hamming code parity bits.
 *  Set *hasError if an error was corrected.
 *  Assumes that data is within range of values which can be decoded
 *  using nParityBits.
 */
HammingWord
hamming_decode(HammingWord encoded, unsigned nParityBits,
                           int *hasError)
{
  int numBits = (2<<nParityBits) - 1;

  // Calculate Error Syndrome
  HammingWord es = 0;
  int pos = 1;
  while(pos < numBits){
	if(get_bit(encoded, pos) != compute_parity(encoded, pos, numBits)) {
		// Bitwise OR into error syndrome
		es |= pos;
	}
	pos <<= 1;
  } 
  if(es){ // Flip bit at Error Syndrome 
  	set_bit(encoded, es, (get_bit(encoded, es)==1) ? 0 : 1);  
	*hasError = 1;
  }

  HammingWord decoded = 0;
  int j = 1;
  for(int i = 1; i <= numBits; i++){
	if(!is_parity_position(i)){
		decoded = set_bit(decoded, j, get_bit(encoded, i));
		j++;
	}
  }

  return decoded;
}










