# "ysim.h"
#include "errors.h"

/************************** Utility Routines ****************************/

/** Return nybble from op (pos 0: least-significant; pos 1:
 *  most-significant)
 */
static Byte
get_nybble(Byte op, int pos) {
  return (op >> (pos * 4)) & 0xF;
}

/************************** Condition Codes ****************************/

/** Conditions used in instructions */
typedef enum {
  ALWAYS_COND, LE_COND, LT_COND, EQ_COND, NE_COND, GE_COND, GT_COND
} Condition;

/** accessing condition code flags */
static inline bool read_cc_flag(Byte cc, unsigned flagBitIndex) {
  return !!(cc & (1 << flagBitIndex));
}
/*static inline bool get_zf(Byte cc) { return read_cc_flag(cc, (1<<ZF_CC)); }
static inline bool get_sf(Byte cc) { return read_cc_flag(cc, (1<<SF_CC)); }
static inline bool get_of(Byte cc) { return read_cc_flag(cc, (1<<OF_CC)); }
*/

static inline bool get_zf(Byte cc) { return (cc & (1<<ZF_CC)) > 0; }
static inline bool get_sf(Byte cc) { return (cc & (1<<SF_CC)) > 0; }
static inline bool get_of(Byte cc) { return (cc & (1<<OF_CC)) > 0; }

/** Return true iff the condition specified in the least-significant
 *  nybble of op holds in y86.  Encoding of Figure 3.15 of Bryant's
 *  CompSys3e.
 */
bool
check_cc(const Y86 *y86, Byte op)
{
  bool ret = false;
  Condition condition = get_nybble(op, 0);
  Byte cc = read_cc_y86(y86);

  switch (condition) {
  case ALWAYS_COND:
    ret = true;
    break;
  case LE_COND:
    ret = (get_sf(cc) ^ get_of(cc)) | get_zf(cc);
    break;
	case LT_COND:
		ret = get_sf(cc) ^ get_of(cc);	
		break;
	case EQ_COND:
		ret = get_zf(cc);
		break;
	case NE_COND:
		ret = !get_zf(cc);
		break;
  case GE_COND:
		ret = !(get_sf(cc)^get_of(cc));
		break;
	case GT_COND:
		ret = !(get_sf(cc)^get_of(cc)) & !get_zf(cc);
		break;	
  default: {
    Address pc = read_pc_y86(y86);
    fatal("%08lx: bad condition code %d\n", pc, condition);
    break;
    }
  }
  return ret;
}

/** return true iff word has its sign bit set */
static inline bool
isLt0(Word word) {
  return (word & (1UL << (sizeof(Word)*CHAR_BIT - 1))) != 0;
}

/** Set condition codes for addition operation with operands opA, opB
 *  and result with result == opA + opB.
 */
static void
set_add_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
	if(result == 0) write_cc_y86(y86, read_cc_y86(y86) | (1<<ZF_CC));
	else write_cc_y86(y86, read_cc_y86(y86) & ~(1<<ZF_CC));

	if(isLt0(result)) write_cc_y86(y86, read_cc_y86(y86) | (1<<SF_CC));
	else write_cc_y86(y86, read_cc_y86(y86) & ~(1<<SF_CC));
	
	// Set overflow if sign changed
	if((isLt0(opA) == isLt0(opB)) && (isLt0(result) != isLt0(opA)))
		write_cc_y86(y86, read_cc_y86(y86) | (1<<OF_CC)); else
		write_cc_y86(y86, read_cc_y86(y86) & ~(1<<OF_CC));
}

/** Set condition codes for subtraction operation with operands opA, opB
 *  and result with result == opA - opB.
 */
static void
set_sub_arith_cc(Y86 *y86, Word opA, Word opB, Word result)
{
	if(result == 0) write_cc_y86(y86, read_cc_y86(y86) | (1<<ZF_CC));
	else write_cc_y86(y86, read_cc_y86(y86) & ~(1<<ZF_CC));
	
	if(isLt0(result)) write_cc_y86(y86, read_cc_y86(y86) | (1<<SF_CC));
	else write_cc_y86(y86, read_cc_y86(y86) & ~(1<<SF_CC));
	
	// Set overflow if sign changed
	if((isLt0(opA) != isLt0(opB)) && (isLt0(result) != isLt0(opB)))
		write_cc_y86(y86, read_cc_y86(y86) | (1<<OF_CC));
  else
		write_cc_y86(y86, read_cc_y86(y86) & ~(1<<OF_CC));
}

static void
set_logic_op_cc(Y86 *y86, Word result)
{
	// Set zero flag
	if(result == 0) write_cc_y86(y86, read_cc_y86(y86) | (1<<ZF_CC)); 
	else write_cc_y86(y86, read_cc_y86(y86) & ~((1<<ZF_CC)));

	// Set sign flag
	if(isLt0(result)) write_cc_y86(y86, read_cc_y86(y86) | (1<<SF_CC));
	else write_cc_y86(y86, read_cc_y86(y86) & ~((1<<SF_CC)));

	// Clear overflow flag
	write_cc_y86(y86, read_cc_y86(y86) & ~(1<<OF_CC));
}

/**************************** Operations *******************************/

static void
op1(Y86 *y86, Byte op, Register regA, Register regB)
{
  enum {ADDL_FN, SUBL_FN, ANDL_FN, XORL_FN };
	switch(op) {
		case ADDL_FN:
		{
 	 		//Add the registers
		  Word reg_a_val = read_register_y86(y86, regA);
			Word reg_b_val = read_register_y86(y86, regB); 	

			Word result = reg_a_val + reg_b_val;
			set_add_arith_cc(y86, reg_a_val, reg_b_val, result);
			write_register_y86(y86, regB, result);

		} break;
	
		case SUBL_FN:
		{
 	 		//Subtract the registers
		  Word reg_a_val = read_register_y86(y86, regA);
			Word reg_b_val = read_register_y86(y86, regB); 	
			
			Word result = reg_b_val - reg_a_val;
			set_sub_arith_cc(y86, reg_a_val, reg_b_val, result);
			write_register_y86(y86, regB, result);

		} break;
	
		case ANDL_FN:
		{
 	 		//AND the registers
		  Word reg_a_val = read_register_y86(y86, regA);
			Word reg_b_val = read_register_y86(y86, regB); 	
			
			Word result = reg_a_val & reg_b_val;
			set_logic_op_cc(y86, result);
			write_register_y86(y86, regB, result);

		} break;

		case XORL_FN:
		{
 	 		//XOR the registers
		  Word reg_a_val = read_register_y86(y86, regA);
			Word reg_b_val = read_register_y86(y86, regB); 	
			
			Word result = reg_a_val ^ reg_b_val;

			set_logic_op_cc(y86, result);
			write_register_y86(y86, regB, result);
		} break;

	}
}

/*********************** Single Instruction Step ***********************/

typedef enum {
  HALT_CODE, NOP_CODE, CMOVxx_CODE, IRMOVQ_CODE, RMMOVQ_CODE, MRMOVQ_CODE,
  OP1_CODE, Jxx_CODE, CALL_CODE, RET_CODE,
  PUSHQ_CODE, POPQ_CODE } BaseOpCode;

/** Execute the next instruction of y86. Must change status of
 *  y86 to STATUS_HLT on halt, STATUS_ADR or STATUS_INS on
 *  bad address or instruction.
 */
void
step_ysim(Y86 *y86)
{

  if(read_status_y86(y86) != STATUS_AOK) return;

  //Fetch instruction
  Address pc = read_pc_y86(y86);    
  if(read_status_y86(y86) != STATUS_AOK) return;

  Byte instr = read_memory_byte_y86(y86, pc);
  if(read_status_y86(y86) != STATUS_AOK) return;

  BaseOpCode op = get_nybble(instr, 1);
  
  switch(op) {
    
		case HALT_CODE:
		{
			write_status_y86(y86, STATUS_HLT);
		} break;

		case NOP_CODE:
 		{
			write_pc_y86(y86, pc + sizeof(Byte));
		} break;

    case IRMOVQ_CODE:
 		{
			Byte reg_byte = read_memory_byte_y86(y86, pc+sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;

			Register reg = get_nybble(reg_byte, 0);	

			Word imm = read_memory_word_y86(y86, pc + 2*sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
	
			write_register_y86(y86, reg, imm);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			write_pc_y86(y86, pc + 2*sizeof(Byte) + sizeof(Word));
  		if(read_status_y86(y86) != STATUS_AOK) return;

		} break;

		case CALL_CODE:
		{
			// Get function address
			Word func_addr = read_memory_word_y86(y86, pc+sizeof(Byte));				
  		if(read_status_y86(y86) != STATUS_AOK) return;

			Word stack_addr = read_register_y86(y86, REG_RSP);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			stack_addr -= sizeof(Address);
			write_register_y86(y86, REG_RSP, stack_addr);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			write_memory_word_y86(y86, stack_addr, pc+sizeof(Byte)+sizeof(Word));
  		if(read_status_y86(y86) != STATUS_AOK) return;


			// Go to the function
			write_pc_y86(y86, func_addr);
	
		} break;

		case RET_CODE:
		{
			// Pop off pc counter from the stack	
			Word stack_addr = read_register_y86(y86, REG_RSP);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			Word ret_addr = read_memory_word_y86(y86, stack_addr);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			stack_addr += sizeof(Address);
			write_register_y86(y86, REG_RSP, stack_addr);
  		if(read_status_y86(y86) != STATUS_AOK) return;
	
			// Move PC to there
			write_pc_y86(y86, ret_addr);			
		} break;

		case MRMOVQ_CODE:
		{
			Byte reg_byte = read_memory_byte_y86(y86, pc + sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
			
			Register reg_a = get_nybble(reg_byte, 1);	
			Register reg_b = get_nybble(reg_byte, 0);	

			Word offset = read_memory_word_y86(y86, pc+2*sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
	
			Word reg_b_value = read_register_y86(y86, reg_b);	
			Word data = read_memory_word_y86(y86, reg_b_value+offset);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			write_register_y86(y86, reg_a, data);

			write_pc_y86(y86, pc+2*sizeof(Byte)+sizeof(Word));

		} break;

		case RMMOVQ_CODE: 
		{
			Byte reg_byte = read_memory_byte_y86(y86, pc + sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
			
			Register reg_a = get_nybble(reg_byte, 1);	
			Register reg_b = get_nybble(reg_byte, 0);	

			Word offset = read_memory_word_y86(y86, pc+2*sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
	
			Word reg_b_value = read_register_y86(y86, reg_b);	
			Word data = read_register_y86(y86, reg_a); 

			write_memory_word_y86(y86, reg_b_value+offset, data);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			write_pc_y86(y86, pc+2*sizeof(Byte)+sizeof(Word));
		} break;
	
		case CMOVxx_CODE:
		{
			Byte func = get_nybble(instr, 0);			

			// Update the pc unconditionally	
			write_pc_y86(y86, pc+2*sizeof(Byte));

			// If the compare fails, skip to next instruction	
			if(!check_cc(y86, func)) break;		

			// Mov value from one register to another
			Byte reg_byte = read_memory_byte_y86(y86, pc+sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
			
			Register reg_a = get_nybble(reg_byte, 1);
			Register reg_b = get_nybble(reg_byte, 0);

			Word reg_a_value = read_register_y86(y86, reg_a);
			write_register_y86(y86, reg_b, reg_a_value);

		} break;

		case OP1_CODE:
		{
			// Mov value from one register to another
			Byte reg_byte = read_memory_byte_y86(y86, pc+sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;
			
			Register reg_a = get_nybble(reg_byte, 1);
			Register reg_b = get_nybble(reg_byte, 0);
				
			op1(y86, get_nybble(instr, 0), reg_a, reg_b);			
			
			write_pc_y86(y86, pc + 2*sizeof(Byte));
		} break;

		case PUSHQ_CODE:
		{
			Byte reg_byte = read_memory_byte_y86(y86, pc+sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;

			Register reg = get_nybble(reg_byte, 1);

			Address stack_addr = read_register_y86(y86, REG_RSP);
			// Make space on the stack
			stack_addr -= sizeof(Address);

			// Push the register value onto the stack
			Word reg_value = read_register_y86(y86, reg);
  		if(read_status_y86(y86) != STATUS_AOK) return;

			write_register_y86(y86, REG_RSP, stack_addr);

			write_memory_word_y86(y86, stack_addr, reg_value);
  		if(read_status_y86(y86) != STATUS_AOK) return;


			write_pc_y86(y86, pc + 2*sizeof(Byte));
		} break;

		case POPQ_CODE:
		{

			Byte reg_byte = read_memory_byte_y86(y86, pc+sizeof(Byte));
  		if(read_status_y86(y86) != STATUS_AOK) return;

			Register reg = get_nybble(reg_byte, 1);

			Address stack_addr = read_register_y86(y86, REG_RSP);

			// Pop the stack into the register
			Word stack_value = read_memory_word_y86(y86, stack_addr);
  		if(read_status_y86(y86) != STATUS_AOK) return;
			stack_addr += sizeof(Address);
			write_register_y86(y86, REG_RSP, stack_addr);

			write_register_y86(y86, reg, stack_value);


			write_pc_y86(y86, pc + 2*sizeof(Byte));

		} break;

		case Jxx_CODE:
		{

			Word dest = read_memory_word_y86(y86, pc+sizeof(Byte));	
			if(check_cc(y86, instr)) write_pc_y86(y86, dest);		
			else write_pc_y86(y86, pc+sizeof(Byte)+sizeof(Word));

		} break;
	
    default: 
    {
      write_status_y86(y86, STATUS_INS);
    } break;

  }   

}

