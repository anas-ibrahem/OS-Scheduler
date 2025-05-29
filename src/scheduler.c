#include "scheduler.h"
#include <stdio.h>
#include <stdlib.h>
#include "DS/IQueue.h"
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include "file_handlers.h"
#include "Algorithms/utils.h"

IQueue* ready_queue;
PCB* current_pcb = NULL;
int quantum = 1;
int msqid_rcv;
int last_remain_time = 0; // Last time the scheduler started a process

int log_start_time = 0; // Start time of the current run

FILE* execution_log = NULL;    // File to log execution periods
int last_process_end_time = 0; // Time when the last process ended or scheduler started
int idle_time = 0;             // CPU idle time

int no_more_processes = 0;
enum SchedulingAlgorithm selected_algorithm;

/**
 * @brief Synchronizes the scheduler with the system clock and starts the scheduling loop.
 * @param sch_algo The scheduling algorithm to use ("rr", "hpf", or "srtn").
 */
void run_scheduler(char* sch_algo, int _quantum) {
    sync_clk();
    printf(BLUE "SCHEDULER: Scheduler synchronized with clock" RESET "\n");
    quantum = _quantum;

    init_scheduler_log();

    execution_log = fopen("execution_log.txt", "w");
    if (execution_log == NULL) {
        perror(RED "Failed to create execution log file" RESET);
        exit(EXIT_FAILURE);
    }

    key_t key = ftok("keyfile", 'A');
    msqid_rcv = msgget(key, IPC_CREAT | 0666);
    if (msqid_rcv == -1) {
        perror(RED "msgget failed in scheduler" RESET);
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, handle_process_termination);
    signal(SIGUSR2, handle_process_tick);

    // Set selected algorithm
    set_selected_algorithm(sch_algo);

    ready_queue = malloc(sizeof(IQueue));
    init_queue(ready_queue, selected_algorithm);

    if (ready_queue == NULL) {
        fprintf(stderr, RED "SCHEDULER: Failed to initialize ready queue" RESET "\n");
        exit(EXIT_FAILURE);
    }

    int current_time = get_clk();
    printf(BLUE "SCHEDULER: Current time: %d, scheduler started!" RESET "\n", current_time);
    last_process_end_time = current_time;

    // Main scheduler loop
    scheduler_loop();
}

/**
 * @brief Runs the scheduler in a continuous loop, managing process execution.
 * @details - checks for new processes
 *          - schedules the next process
 *          - handles the current running process.
 */
void scheduler_loop() {
    while (no_more_processes == 0 || !is_empty(ready_queue) || current_pcb != NULL) {
        check_for_new_processes();

        if (current_pcb == NULL) {
            int current_time = get_clk();
            if (current_time > last_process_end_time && !is_empty(ready_queue)) {
                fprintf(execution_log, "Idle from %d → %d\n\n", last_process_end_time, current_time);
                fflush(execution_log);

                idle_time += current_time - last_process_end_time;
            }
            schedule_next_process();
        }
        handle_current_running_process();

        // check_no_more_processes(); // removing repeated call temp
        check_no_more_processes();

        usleep(50000); // Sleep for 50ms to avoid busy-waiting
    }

    scheduler_perf_out(idle_time, get_clk());
    sleep(1);
}

/**
 * @brief Sets the scheduling algorithm based on the input string.
 * @param scheduling_algorithm The name of the scheduling algorithm ("rr", "hpf", "srtn")
 *          - "rr" → Round Robin
 *          - "hpf" → Highest Priority First
 *          - "srtn" → Shortest Remaining Time Next
 */
void set_selected_algorithm(char* scheduling_algorithm) {
    if (strcmp(scheduling_algorithm, "rr") == 0) {
        printf(BLUE "SCHEDULER: Using Round Robin scheduling with quantum = %d" RESET "\n", quantum);
        selected_algorithm = RR;
    }
    if (strcmp(scheduling_algorithm, "hpf") == 0) {
        printf(BLUE "SCHEDULER: Using Highest Priority First scheduling" RESET "\n");
        selected_algorithm = HPF;
    }
    if (strcmp(scheduling_algorithm, "srtn") == 0) {
        printf(BLUE "SCHEDULER: Using Shortest Remaining Time Next scheduling" RESET "\n");
        selected_algorithm = SRTN;
    }
}

/**
 * @brief Checks for new processes sent to the scheduler via message queue.
 *        Adds valid processes to the ready queue.
 */
void check_for_new_processes() {
    msg_struct msg;
    int recv_status;

    // Clear message buffer
    while (1) {
        memset(&msg, 0, sizeof(msg_struct));
        usleep(15000);

        // Read the message
        recv_status = msgrcv(msqid_rcv, &msg, sizeof(msg.data.pcb), 1, IPC_NOWAIT);

        if (recv_status == -1) {
            return;
        }

        int current_time = get_clk();
        printf(BLUE "SCHEDULER: Current time: %d, process %d (PID: %d) is received" RESET "\n",
            current_time, msg.data.pcb.PDATA.id, msg.data.pcb.pid);

        if (msg.data.pcb.pid <= 0 || msg.data.pcb.PDATA.id <= 0 || msg.data.pcb.PDATA.runtime <= 0) {
            fprintf(stderr, RED "SCHEDULER: Invalid process data received" RESET "\n");
            return;
        }

        // create new pcb
        PCB* new_pcb = malloc(sizeof(PCB));

        if (new_pcb == NULL) {
            perror(RED "SCHEDULER: Failed to allocate memory for new PCB entry" RESET);
            exit(EXIT_FAILURE);
        }

        *new_pcb = msg.data.pcb;

        printf("inside srtn");

        // debug lines
        printf("Debug: Received message - Process ID: %d, Arrival Time: %d, Runtime: %d, Received PID: %d\n",
            msg.data.pcb.PDATA.id, msg.data.pcb.PDATA.arrival_time, msg.data.pcb.PDATA.runtime, msg.data.pcb.pid);

        switch (selected_algorithm) {
        case SRTN:
            enqueue(ready_queue, new_pcb, new_pcb->remaining_time);
            break;

        case HPF:
            enqueue(ready_queue, new_pcb, new_pcb->PDATA.priority);
            break;

        case RR:
            enqueue(ready_queue, new_pcb, 0); // RR doesn't use priority
            break;
        }
        printf(BLUE "SCHEDULER: Process %d added to the ready queue" RESET "\n", new_pcb->PDATA.id);
    }
}

/**
 * @brief Handles the execution of the currently running process based on the selected scheduling algorithm.
 */
void handle_current_running_process() {
    switch (selected_algorithm) {
    case RR:
        if (current_pcb != NULL && current_pcb->remaining_time <= last_remain_time - quantum) {
            current_pcb->remaining_time = last_remain_time - quantum;
            handle_quantum_expiration(0);
        }
        break;

    case HPF:
        if (current_pcb == NULL) {
            schedule_next_process();
        }
        break;

    case SRTN:
        // Process* next_process = (Process*)front(ready_queue);
        // Process* next_process = &next_pcb->process;
        PCB* next_pcb = (PCB*)front(ready_queue);
        if (next_pcb == NULL) {
            return;
        }

        int next_id = next_pcb->PDATA.id;
        int next_pid = next_pcb->pid;

        if (next_pcb != NULL && current_pcb != NULL && next_pcb != current_pcb) {

            printf(GREEN "SCHEDULER: Next process in queue: %d (PID: %d) TR: %d, current process TR: %d queue size: %d" RESET "\n",
                next_id, next_pid, next_pcb->remaining_time, current_pcb->remaining_time, size(ready_queue));

            printf(BLUE "SCHEDULER: Process %d (PID: %d) has a shorter remaining time than current process %d (PID: %d) (%d < %d)" RESET "\n",
                next_id, next_pid, current_pcb->PDATA.id, current_pcb->pid, next_pcb->remaining_time, current_pcb->remaining_time);

            handle_quantum_expiration(0);
        }
        break;
    }
}

/**
 * @brief Schedules the next process to run based on the selected scheduling algorithm.
 */

void schedule_next_process() {
    if (is_empty(ready_queue)) {
        return;
    }

    if (selected_algorithm == HPF) {
        // current_running_process = (Process*)dequeue(ready_queue, 1); // remove entirly from ready queue
        current_pcb = (PCB*)dequeue(ready_queue, 1);
    }
    else {
        // current_running_process = (Process*)front(ready_queue);
        current_pcb = (PCB*)front(ready_queue);
    }

    printf(BLUE "SCHEDULER: Scheduling process %d (PID: %d) with remaining time %d" RESET "\n",
        current_pcb->PDATA.id, current_pcb->pid, current_pcb->remaining_time);

    if (current_pcb == NULL) {
        return;
    }

    int current_time = get_clk();
    printf(BLUE "SCHEDULER: Current time: %d, running process %d (PID: %d)" RESET "\n",
        current_time, current_pcb->PDATA.id, current_pcb->pid);

    if (current_pcb->start_time == -1) {
        current_pcb->state = "started";
        current_pcb->start_time = current_time;
        current_pcb->wait_time = current_time - current_pcb->PDATA.arrival_time;
    }
    else {
        current_pcb->state = "resumed";
        current_pcb->wait_time =
            (current_time - current_pcb->PDATA.arrival_time) - (current_pcb->PDATA.runtime - current_pcb->remaining_time);
    }

    // Start or resume the process
    kill(current_pcb->pid, SIGCONT);
    last_remain_time = current_pcb->remaining_time;
    // Set the process start time and quantum end time based on system clock
    log_start_time = current_time;

    log_event(current_pcb);

    if (selected_algorithm == RR) {
        printf(BLUE "SCHEDULER: Quantum will expire at time %d" RESET "\n", current_time + quantum);
    }
}

/**
 * @brief Handles the termination of the currently running process.
 * @param signum The signal number (expected to be SIGUSR1).
 * @details This function is called when the currently running process terminates.
 */
void handle_process_termination(int signum) {
    int current_time = get_clk();

    printf(GREEN "SCHEDULER: Process %d (PID: %d) has completed execution at time %d" RESET "\n",
        current_pcb->PDATA.id, current_pcb->pid, current_time);

    fprintf(execution_log, "P%d (burst %d) runs %d → %d and finishes at %d\n\n",
        current_pcb->PDATA.id,
        current_pcb->PDATA.runtime,
        log_start_time,
        current_time,
        current_time);
    fflush(execution_log);

    last_process_end_time = current_time;

    if (selected_algorithm != HPF) {
        dequeue(ready_queue, 1);
    }

    if (!is_empty(ready_queue)) {
        printf("SCHEDULER: Context Switching to next process with id %d\n", ((Process*)front(ready_queue))->id);
    }
    else {
        printf(BLUE "SCHEDULER: No more processes in the queue" RESET "\n");
    }

    current_pcb->remaining_time--;
    current_pcb->state = "finished";
    log_event(current_pcb);

    current_pcb = NULL;
    last_remain_time = 0; // Reset last start time

    // Reset quantum tracking variables
    log_start_time = 0;
}

/**
 * @brief Handles the expiration of the quantum for the currently running process.
 * @param signum The signal number NOT USED FOR NOW.
 * @details This function is called when the quantum expires for the currently running process.
 */
void handle_quantum_expiration(int signum) {
    if (current_pcb == NULL) {
        return;
    }
    int current_time = get_clk();
    printf(BLUE "SCHEDULER: Quantum expired for process %d at time %d" RESET "\n",
        current_pcb->PDATA.id, current_time);

    fprintf(execution_log, "P%d (burst %d) runs %d → %d (rem %d)\n\n",
        current_pcb->PDATA.id,
        current_pcb->PDATA.runtime,
        log_start_time,
        current_time,
        current_pcb->remaining_time);
    fflush(execution_log);

    last_process_end_time = current_time;

    kill(current_pcb->pid, SIGSTOP);

    current_pcb->state = "stopped";
    log_event(current_pcb);

    current_pcb = NULL;
    usleep(1000); // Sleep for 1ms to ensure the process is stopped

    if (selected_algorithm == RR)
        dequeue(ready_queue, 0);

    // Reset quantum tracking variables
    log_start_time = 0;
    // Reset current process
}

/**
 * @brief Handles the tick signal for the currently running process.
 * @param signum The signal number (not used in this function).
 * @details This function decrements the remaining time of the current process.
 */
void handle_process_tick(int signum) {
    printf(BLUE "SCHEDULER: Tick received for process %d (PID: %d)" RESET "\n",
        current_pcb->PDATA.id, current_pcb->pid);

    if (current_pcb == NULL) {
        return;
    }

    int current_time = get_clk();
    current_pcb->remaining_time--;

    switch (selected_algorithm) {
    case SRTN:
        change_priority(ready_queue, 0, current_pcb->remaining_time);
        break;
    default:
        break;
    }

    printf("SCHEDULER: Process %d (PID: %d) ticked at time %d, remaining time: %d\n",
        current_pcb->PDATA.id, current_pcb->pid, current_time, current_pcb->remaining_time);
}

/**
 * @brief Check no more processes message from process generator
 */
void check_no_more_processes() {
    msg_struct msg;
    int recv_status;

    // Clear message buffer
    memset(&msg, 0, sizeof(msg_struct));

    recv_status = msgrcv(msqid_rcv, &msg, sizeof(msg.data.str), 2, IPC_NOWAIT);

    if (recv_status == -1) {
        return;
    }

    if (strcmp(msg.data.str, "no_more_processes") == 0) {
        no_more_processes = 1;
    }
}