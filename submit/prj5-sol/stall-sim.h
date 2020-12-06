#ifndef _STALL_SIM
#define _STALL_SIM

#include "y86x.h"

/** An opaque structure which tracks pipeline stall state.
 */
typedef struct StallSimStruct StallSim;

/** Create a new pipeline stall simulator for y86. */
StallSim *new_stall_sim(Y86 *y86);

/** Free all resources allocated by new_pipe_sim() in stallSim. */
void free_stall_sim(StallSim *stallSim);

/** Apply next pipeline clock to stallSim.  Return true if
 *  processor can proceed, false if pipeline is stalled.
 *  Any Y86 state contained in stallSim must not be changed
 *  by this function.
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
 * written by any of upto 3 preceeding instructions.  This applies
 * to conditional moves irrespective of the value of the condition.
 */
bool clock_stall_sim(StallSim *stallSim);

#endif //ifndef _STALL_SIM_H
