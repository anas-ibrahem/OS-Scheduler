#ifndef PCB_H
#define PCB_H

#include "process.h"
#include "buddy_memory.h"
// PCB represents the runtime state of a process
typedef struct PCB
{
    int pid; // from system fork

    // These values are updated during the simulation
    // NOTE tunraround & WTA to be calculated by the end of the process
    // TA = wait_time + runtime
    // WTA = TA / runtime
    int start_time;
    int wait_time;
    int remaining_time; // initially set to runtime
    char* state;

    Block * memory_block;

    struct Process PDATA; // data of process read by input file
} PCB;

#endif