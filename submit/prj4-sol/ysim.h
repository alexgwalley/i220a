#ifndef _YSIM_H
#define _YSIM_H

#include "y86.h"

/** Execute the next instruction of y86. Must change status of
 *  y86 to STATUS_HLT on halt, STATUS_ADR or STATUS_INS on
 *  bad address or instruction.
 */
void step_ysim(Y86 *y86);

#endif //ifndef _YSIM_H
