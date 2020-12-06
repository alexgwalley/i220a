#ifndef _Y86_H
#define _Y86_H

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>   //for C ints of specified width
#include <stdio.h>

enum { DEFAULT_Y86_MEMORY_SIZE = 8 * 1024 };

#define BYTE_BITS CHAR_BIT

typedef uint8_t Byte;
typedef uint64_t Word;
typedef uint64_t Address;
typedef uint64_t Size;

typedef enum {
  REG_RAX, REG_RCX, REG_RDX, REG_RBX, REG_RSP, REG_RBP, REG_RSI, REG_RDI,
  REG_R8, REG_R9, REG_R10, REG_R11, REG_R12, REG_R13, REG_R14, REG_NONE,
  N_REG = REG_NONE
} Register;

/** Flag bit positions for condition bits in cc word */
enum {
  OF_CC,   //overflow flag
  SF_CC,   //sign flag
  ZF_CC    //zero flag
};

/** machine status */
typedef enum {
  STATUS_AOK = 1,   //everything ok, machine is running
  STATUS_HLT,       //machine halted by a halt instruction.
  STATUS_ADR,       //machine stopped because a bad address.
  STATUS_INS        //machine stopped because of a bad instruction
} Status;

/** Y86 is an opaque struct */
typedef struct Y86Struct Y86;

/** Create a new uninitialized Y86 instance */
Y86 *new_y86(Size memorySize);

/** Create a new uninitialized Y86 instance with a default memory size. */
Y86 *new_y86_default(void);

/** Free all resources used by y86. */
void free_y86(Y86 *y86);

/** Return size of y86 memory. */
Size get_memory_size_y86(const Y86 *y86);

/** Return contents of register reg in y86. */
Word read_register_y86(const Y86 *y86, Register reg);

/** Set contents of register reg in *y86 to value. */
void write_register_y86(Y86 *y86, Register reg, Word value);

/** Return contents y86 program-counter. */
Address read_pc_y86(const Y86 *y86);

/** Set y86 program-counter to addr. Sets y86 status to STATUS_ADR if
 *  addr is invalid.
 */
void write_pc_y86(Y86 *y86, Address addr);

/** Return y86 status. */
Status read_status_y86(const Y86 *y86);

/** Set y86 status to status. */
void write_status_y86(Y86 *y86, Status status);

/** Return contents y86 condition-code. */
Byte read_cc_y86(const Y86 *y86);

/** Set y86 condition-code to cc. */
void write_cc_y86(Y86 *y86, Byte cc);

/** Returns byte at memory address addr in y86.  Sets y86 status to
 *  STATUS_ADR if addr is invalid.
 */
Byte read_memory_byte_y86(Y86 *y86, Address addr);

/** Set value of byte at memory address addr in y86 to value.  Sets
 *  y86 status to STATUS_ADR if addr is invalid.
 */
void write_memory_byte_y86(Y86 *y86, Address addr, Byte value);

/** Returns value of word (little-endian) at memory address addr in
 *  y86.  Sets y86 status to STATUS_ADR if addr is invalid.
 */
Word read_memory_word_y86(Y86 *y86, Address addr);

/** Set value of word (little endian) at memory address addr in y86 to
 *  value.  Sets y86 status to STATUS_ADR if addr is invalid.
 */
void write_memory_word_y86(Y86 *y86, Address addr, Word value);

/** Return pointer to memory byte at specified address addr.  Sets y86
 *  status to STATUS_ADR if addr is invalid.  Note that writing
 *  through this pointer will not be captured for dump_changes_y86().
 */
Byte *get_memory_pointer_y86(Y86 *y86, Address addr);

/** Write lines containing key: value (in hex) pairs to out giving
 *  changes to y86 state since last call.  If isVerbose is true,
 *  include all registers, condition-code, and machine status
 *  (irrespective of change), otherwise include only changes.  Keys
 *  include register-names eax, ebx, ecx, edx, ebp, esp, esi, edi, cc
 *  (for condition-code) and status (for machine status).  Changes to
 *  memory at addr (hex) use key B[addr] (for byte updates) or W[addr]
 *  (for word updates).
 */
void dump_changes_y86(const Y86 *y86, bool isVerbose, FILE *out);

#endif //ifndef _Y86_H
