#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <sys/ipc.h> // For IPC_CREAT, etc.
#include <sys/msg.h> // For message queue functions

#include "clk.h"
#include "process.h"
#include "PCB.h"

// Color Macros
#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

// Buffer struct for sending/receiving process structs
typedef struct msgbuf
{
    long mtype;
    union
    {
        // Process proc;           // Process structure
        PCB pcb;
        char str[18]; // String message
    } data;
} msg_struct;

void run_scheduler(char* sch_algo, int _quantum); // Starts the schedular
void scheduler_loop(); // Main scheduler loop

void set_selected_algorithm(char* scheduling_algorithm); // Function to select the algorithm
void check_for_new_processes(); // Check for new processes in the  ready queue
void handle_current_running_process(); // Handle the currently running process based on selected algo
void schedule_next_process(); // Schedules the next process

// Signal handler function 
void handle_process_termination(int signum); // Handle process termination
void handle_quantum_expiration(int signum);  // Handle quantum expiration
void handle_process_tick(int signum);        // Handle process tick
void check_no_more_processes();              // Check for no_more_processes message

#endif