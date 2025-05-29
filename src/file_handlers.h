#ifndef FILE_HANDLERS_H
#define FILE_HANDLERS_H

#include "process.h"
#include "PCB.h"

int read_input_file(const char *filename, Process **process_list, int *process_count);
// exit code 0 for success, 1 for error

void init_scheduler_log();
void log_event(PCB *pcb);
void scheduler_perf_out(int total_idle, int total_time);
char *round_number(double number);

void init_memory_log();
void log_memory_event(int pid, Block* block, int type);

#endif