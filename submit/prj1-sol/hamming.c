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
  return (word >> bitIndex)&1;
}

/** Return word with bit at bitIndex in word set to bitValue. */
static inline HammingWord
set_bit(HammingWord word, int bitIndex, unsigned bitValue)
{
  assert(bitIndex > 0);
  assert(bitValue == 0 || bitValue == 1);
  return (bitValue) ? word | (1<<bitIndex) : word & ~(1<<bitIndex);
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
	num_bits_set = 0;
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
  //@TODO
  return 0;
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
  //@TODO
  return 0;
}
