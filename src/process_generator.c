#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>    // for fork, execl
#include <sys/types.h> // for pid_t
#include <sys/ipc.h>   // For IPC_CREAT, etc.
#include <sys/msg.h>   // For message queue functions
#include <sys/wait.h>  // for waitpid

#include "clk.h"
#include "process.h"
#include "scheduler.h"
#include "file_handlers.h"
#include "DS/IQueue.h"
#include "PCB.h"
#include "buddy_memory.h"
#include "DS/linked_list.h"
#include "memory_manager.h"

void handle_child_exit(int signum);
void clear_and_exit(int);
void fork_and_send(Process * proc , Block * allocated_block);
void create_processes();
void parse_args(int argc, char* argv[], char** scheduling_algo, char** input_file, int* quantum);
pid_t clk_pid = -1;
int sch_pid = -1; // Scheduler process id
 

Process* process_list = NULL; // List of processes
int process_count = 0;        // Number of processes

int msqid;   // Message queue id

LinkedList *waiting_list = NULL;

int main(int argc, char* argv[]) {

    printf("PCB size: %zu, Process size: %zu\n", sizeof(PCB), sizeof(Process));

    char* scheduling_algorithm; // Scheduling algorithm
    char* input_file;           // Input file name
    int quantum;

    // Set arguments
    parse_args(argc, argv, &scheduling_algorithm, &input_file, &quantum);

    clk_pid = fork();
    if (clk_pid == 0) // Child (CLK)
    {
        init_clk();
        sync_clk();
        run_clk();
    }
    else // Process Generator
    {
        signal(SIGINT, clear_and_exit);
        signal(SIGCHLD , handle_child_exit);
        sync_clk();

        if (read_input_file(input_file, &process_list, &process_count) == 0) {
            printf(MAGENTA "P GEN  : Processes read from file Succesfully" RESET "\n");
        }
        else {
            fprintf(stderr, MAGENTA "P GEN  : Error reading input file." RESET "\n");
            exit(EXIT_FAILURE);
        }

        printf(MAGENTA "P GEN  : Scheduling algorithm selected: %s" RESET "\n", scheduling_algorithm);

        // Fork scheduler
        sch_pid = fork();
        if (sch_pid == -1) {
            fprintf(stderr, MAGENTA "P GEN  : Error in fork" RESET "\n");
            exit(EXIT_FAILURE);
        }
        else if (sch_pid == 0) // Child process
        {
            signal(SIGINT, SIG_DFL);
            run_scheduler(scheduling_algorithm, quantum);
            return 0; // Scheduler process
        }

        // Create message queue to send processes
        key_t key = ftok("keyfile", 'A');
        msqid = msgget(key, IPC_CREAT | 0666);

        if (msqid == -1) {
            perror("msgget failed");
            exit(1);
        }

        // Create the processes in their arrival time
        init_memory_manager();
        waiting_list = create_linked_list();
        create_processes();

        int status;
        waitpid(sch_pid, &status, 0);

        destroy_clk(1);
    }
}

/**
 * @brief Set program arguments (scheduling algorithm and input file)
 *
 * @param[in] int argc, char *argv[], char **scheduling_algorithm and char **input_file
 */
void parse_args(int argc, char* argv[], char** scheduling_algo, char** input_file, int* quantum) {
    // Check the count of arguments and -s flag
    if (argc != 5 && argc != 7 || strcmp(argv[1], "-s") != 0) {
        fprintf(stderr, MAGENTA "P GEN  : Usage: ./os-sim -s <scheduling-algorithm> [-q <quantum>] -f <processes-text-file>" RESET "\n");
        exit(EXIT_FAILURE);
    }

    *scheduling_algo = argv[2];

    // Check the algorithm
    if (strcmp(*scheduling_algo, "rr") != 0 && strcmp(*scheduling_algo, "hpf") != 0 && strcmp(*scheduling_algo, "srtn") != 0) {
        fprintf(stderr, MAGENTA "P GEN  : Please enter a valid algorithm!" RESET "\n");
        fprintf(stderr, MAGENTA "P GEN  : Valid algorithms: (rr - hpf - srtn)" RESET "\n");
        exit(EXIT_FAILURE);
    }

    // Check for algorithm arguments
    if (strcmp(*scheduling_algo, "rr") == 0) {
        if (argc != 7 || strcmp(argv[3], "-q") != 0 || strcmp(argv[5], "-f") != 0) {
            fprintf(stderr, "Algorithm Usage: ./os-sim -s rr -q <quantum> -f <processes-text-file>\n");
            exit(EXIT_FAILURE);
        }

        *quantum = atoi(argv[4]);
        if (*quantum <= 0) {
            fprintf(stderr, "Quantum must be a positive integer.\n");
            exit(EXIT_FAILURE);
        }
        *input_file = argv[6];
    }
    else {
        if (argc != 5 || strcmp(argv[3], "-f") != 0) {
            fprintf(stderr, "Usage: ./os-sim -s <scheduling-algorithm> [-q <quantum>] -f <processes-text-file>\n");
            exit(EXIT_FAILURE);
        }
        *input_file = argv[4];
    }
}

volatile sig_atomic_t interrupted = 0;
/**
 * @brief Create all the processes read from input file in their arrival time
 * and send them to the scheduler
 */
void create_processes() {
    int* processed = calloc(process_count, sizeof(int));
    int i = 0;
    Block * fit_memory_block = NULL;
    Process* waited_fit = NULL;

    
    while ( (i < process_count || waiting_list->size != 0) && !interrupted) 
    {
        waited_fit = get_oldest_fit(waiting_list);
        while (waited_fit != NULL) // waiting list
        {
            fit_memory_block = get_smallest_block(waited_fit);
            if(fit_memory_block != NULL)
            {
                Block* allocated_block = allocate_process_memory(waited_fit->memory_size);
                printf("Sending to sched from wait queue %d\n", waited_fit->id);
                fork_and_send(waited_fit, fit_memory_block);
            }
            else {
                printf("NO OLDEST FIT MEMORY");
            }
            waited_fit = get_oldest_fit(waiting_list);
        }
        
        if (i < process_count && get_clk() == process_list[i].arrival_time && !processed[i]) 
        {
            processed[i] = 1;
            Process* proc = &process_list[i];
            Block* allocated_block = allocate_process_memory(proc->memory_size);
            if (allocated_block == NULL)
            {
                printf(MAGENTA "P GEN  : Current time: %d, process %d will be added to waiting list" RESET "\n", get_clk(), proc->id);
                Node* node = create_node(proc);
                add_node_to_back(waiting_list, node);
                i++;
                continue;
            }
            printf("Sending to sched from ready queue %d\n", proc->id);
            fork_and_send(proc, allocated_block);

            printf(MAGENTA "P GEN  : Current time: %d, process %d will be sent to schedular" RESET "\n", get_clk(), proc->id);
            
            i++;
        }
        else
        {
            usleep(50000);
        }
    }

    // Send "no_more_processes" to scheduler
    if (waiting_list->size == 0)
       {
        printf("Sending message");
        msg_struct msg;
        memset(&msg, 0, sizeof(msg_struct));
        msg.mtype = 2;

        strncpy(msg.data.str, "no_more_processes", 17);
        msg.data.str[17] = '\0';

        if (msgsnd(msqid, &msg, sizeof(msg.data.str), 0) == -1) {
            perror(RED "P GEN  : msgsnd failed" RESET);
        }

        free(processed);
       }
}


void fork_and_send(Process * proc , Block * allocated_block)
{
    int pid = fork();

    if (pid == -1) {
        fprintf(stderr, RED "P GEN  : Error in fork" RESET "\n");
        return;
    }
    else if (pid == 0) // Child process
    {
        signal(SIGINT, SIG_DFL); // Ignore SIGINT in child process
        run_process(proc->runtime, sch_pid); // Start execution
    }
    // parent process -> process generator
    else {
        // create new PCB entry for the current process
        PCB new_pcb;

        new_pcb.pid = pid; // pid
        new_pcb.PDATA = *proc;
        new_pcb.remaining_time = proc->runtime; // initialized to run time
        new_pcb.start_time = -1;
        new_pcb.wait_time = 0;

        assign_memory_block_to_process(&new_pcb, allocated_block);

        printf("Debug: Sending process (ID: %d, Arrival Time: %d, Runtime: %d, PID: %d) to scheduler\n",
            new_pcb.PDATA.id, new_pcb.PDATA.arrival_time, new_pcb.PDATA.runtime, new_pcb.pid);

        // send message
        msg_struct msg;
        memset(&msg, 0, sizeof(msg_struct));

        msg.mtype = 1;
        msg.data.pcb = new_pcb;

        if (msgsnd(msqid, &msg, sizeof(msg.data.pcb), 0) == -1) {
            perror(RED "P GEN  : msgsnd failed" RESET);
            kill(pid, SIGKILL); // Kill the child process on send failure
        }

    }
}

/**
 * @brief Signal handler to clear resources
 * @details: This function is called when the process generator receives a SIGINT signal.
 * It clears the message queue and terminates all child processes.
 * Finally it terminates the
 * @param[in] int signum
 */

void clear_and_exit(int signum) {
    interrupted = 1;
    signal(SIGINT, SIG_IGN); // Process generator ignores the next signal to avoid clearing twice
    free_linked_list(waiting_list);
    killpg(getpgrp(), SIGINT); // Interrupt all children including the clock
    free_memory_manager();
    // Clear message queue
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror(RED "P GEN  : Cleaning up (msgctl) failed" RESET);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}


void handle_child_exit(int signum) {
    int status;
    pid_t exited_pid;

    // Wait for any process to exit without blocking
    while ((exited_pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (exited_pid != clk_pid && exited_pid != sch_pid) {
            printf(MAGENTA "P GEN  : Process exited (PID: %d)" RESET "\n", exited_pid);
            if(deallocate_process_memory(exited_pid) == 1) {
                printf(MAGENTA "P GEN  : Memory deallocated for process %d" RESET "\n", exited_pid);
            }
            else {
                printf(MAGENTA "P GEN  : Error deallocating memory for process %d" RESET "\n", exited_pid);
            }

        }
    }
}