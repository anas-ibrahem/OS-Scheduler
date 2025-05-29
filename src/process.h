#ifndef PROCESS_H
#define PROCESS_H

void handle_sigcont(int signum);
void run_process(int runtime, int sch_pid);
void init_process(int runtime, int sch_pid);

// process struct mainly for the I/O file management
// it represents the input data of process 
typedef struct Process
{
    int id;  // READ from file
    int arrival_time;
    int runtime;
    int priority; // 0 serves as highest priority
    int memory_size; // needed memory by process - max is 256
} Process;

#endif