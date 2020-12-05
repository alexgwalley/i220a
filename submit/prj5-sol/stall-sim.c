#include "stall-sim.h"

#include "y86-util.h"

#include "errors.h"
#include "memalloc.h"

#include <assert.h>

enum {
  MAX_DATA_BUBBLES = 3,  /** max # of bubbles due to data hazards */
  JUMP_BUBBLES = 2,      /** # of bubbles for cond jump op */
  RET_BUBBLES = 3,       /** # of bubbles for return op */
  MAX_REG_WRITE = 2      /** max # of registers written per clock cycle */
};

struct StallSimStruct {
	Y86* y86;
	int numBubbles;
	bool stalling;
	Byte regsToInsert;
	Byte regs[MAX_DATA_BUBBLES];
};


/********************** Allocation / Deallocation **********************/

/** Create a new pipeline stall simulator for y86. */
StallSim *
new_stall_sim(Y86 *y86)
{
	StallSim* stallSim = malloc(sizeof(StallSim));
	stallSim->y86 = y86;
	stallSim->numBubbles = 4;
	stallSim->stalling = false;
	for(int i = 0; i < MAX_DATA_BUBBLES; i++){
		stallSim->regs[i] = 0xee;
  } 
	stallSim->regsToInsert = 0xee;
  return stallSim;
}

/** Free all resources allocated by new_pipe_sim() in stallSim. */
void
free_stall_sim(StallSim *stallSim)
{
	free(stallSim);
}

void print_regs(StallSim* stallSim) {
	printf("\nregs[0]: %x\n", stallSim->regs[0]);
	printf("regs[1]: %x\n", stallSim->regs[1]);
	printf("regs[2]: %x\n", stallSim->regs[2]);
}

Byte swap_nybbles(Byte b){
	Byte res = 0;
	res |= get_nybble(b, 1);
	res |= get_nybble(b, 0)<<4;	
	return res;
}

/**
 * Returns the number of bubbles needed to wait until pipeline catches up
 * so that there are no memory issues	
 */
int
num_bubbles_from_prev_regs(Byte regs, StallSim* stallSim, bool testBoth) {
	for(int i = 0; i < MAX_DATA_BUBBLES; i++) {
		
		if(get_nybble(regs, 1) == get_nybble(stallSim->regs[i], 0) ||
				get_nybble(regs, 0) == get_nybble(stallSim->regs[i], 0)){
				return MAX_DATA_BUBBLES - i;
		}
		if(testBoth && 
				get_nybble(regs, 0) == get_nybble(stallSim->regs[i], 1)) {

				return MAX_DATA_BUBBLES - i;
		}
	}	

	return 0;	
}

void
shift_regs(StallSim* stallSim, Byte regs) {
		stallSim->regs[2] = stallSim->regs[1];
		stallSim->regs[1] = stallSim->regs[0];
		stallSim->regs[0] = regs;
}

/** Apply next pipeline clock to stallSim.  Return true if
 *  processor can proceed, false if pipeline is stalled.
 *
 * The pipeline will stall under the following circumstances:
 *
 * Exactly 4 clock cycles on startup to allow the pipeline to fill up.
 *
 * Exactly 2 clock cyclies after execution of a conditional jump.
 *
 * Exactly 3 clock cycles after execution of a return.
 *
 * Upto 3 clock cycles when attempting to read a register which was
 * written by any of upto 3 preceeding instructions.
 */
bool
clock_stall_sim(StallSim *stallSim)
{
	Address pc_addr = read_pc_y86(stallSim->y86);
	Byte instr = read_memory_byte_y86(stallSim->y86, pc_addr);
  Byte opCode = get_nybble(instr, 1);	

	Byte regs = 0xee;
	if(!stallSim->stalling && stallSim->numBubbles <= 0) {
		int num_bubbles_to_add = 0;
		switch (opCode) {
			case Jxx_CODE:
			{
				
				Byte type = read_memory_byte_y86(stallSim->y86, pc_addr) & 0xf;
				if(type != 0)
					num_bubbles_to_add += JUMP_BUBBLES;
			} break;	
			case RET_CODE:
			{
				num_bubbles_to_add += RET_BUBBLES;
			} break;
			case NOP_CODE:
			{
			}	break;
			case IRMOVQ_CODE:
			{
				regs = read_memory_byte_y86(stallSim->y86, pc_addr+sizeof(Byte));
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, false);					  	
			} break;
			case RMMOVQ_CODE:
			{
			} break;
			case CMOVxx_CODE:
			{
				regs = read_memory_byte_y86(stallSim->y86, pc_addr+sizeof(Byte));
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, false);					  	
			} break;
			case MRMOVQ_CODE:
			{
				regs = read_memory_byte_y86(stallSim->y86, pc_addr+sizeof(Byte));
				regs = swap_nybbles(regs);
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, true);					  	
			} break;
			case OP1_CODE:
			{
				regs = read_memory_byte_y86(stallSim->y86, pc_addr+sizeof(Byte));
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, true);					  	
			} break;
			case PUSHQ_CODE:
			{
				regs = read_memory_byte_y86(stallSim->y86, pc_addr+sizeof(Byte)) & 0xf0;
				regs |= 4; // rsp
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, false);					  	
			} break;
			case POPQ_CODE:
			{
				regs = read_memory_byte_y86(stallSim->y86, pc_addr+sizeof(Byte)) & 0xf0;
				regs |= 4; // rsp
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, false);					  	
			} break;
			case CALL_CODE:
			{
				regs = 0xf4;	
				num_bubbles_to_add += num_bubbles_from_prev_regs(regs, stallSim, false);					  	
			} break;
			default:
			  break;
		}


		if(num_bubbles_to_add > 0){
			stallSim->numBubbles += num_bubbles_to_add;
			stallSim->regsToInsert = regs;
			stallSim->stalling = true;
		}
		shift_regs(stallSim, regs);
#if DEBUG
		print_regs(stallSim);
#endif
	}

	if(stallSim->numBubbles > 0){
		stallSim->numBubbles--;	
		if(stallSim->numBubbles == 0) {
			shift_regs(stallSim, stallSim->regsToInsert);
			stallSim->regsToInsert = 0xee;
		}
		else shift_regs(stallSim, 0xee);
		return false;
	}

	stallSim->stalling = false;

  return true;
}



