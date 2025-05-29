#include "clk.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h> // Added for getpid() function
#include "process.h"

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

int process_runtime;
int SCH_PID;
int elapsed = 0;
int last_clk = 0;

/**
 * @brief Signal handler for SIGCONT signal.
 * @details This function is called when the process receives a SIGCONT signal.
 * It re-syncs with the clock and resumes the process execution.
 * @param signum The signal number (expected to be SIGCONT).
 */
void handle_sigcont(int signum)
{
    // Re-sync with clock when the process is continued
    sync_clk();
    last_clk = get_clk(); // Reset last_clk to current time to avoid immediate tick
    printf(YELLOW "PROCESS (PID %d): Re-synchronized with clock at time %d" RESET "\n", getpid(), last_clk);
}

/**
 * @brief Runs the process for the specified runtime.
 * @details This function simulates the process execution
 * It also handles the SIGUSR1 signal to notify the scheduler when the process finishes execution.
 * @param runtime The runtime of the process.
 * @param sch_pid The PID of the scheduler process.
 */
void run_process(int runtime, int sch_pid)
{
    // sync_clk();
    process_runtime = runtime;
    SCH_PID = sch_pid;

    // Set up signal handlers
    signal(SIGCONT, handle_sigcont);
    signal(SIGSTOP, SIG_DFL);

    // Initial stop to wait for scheduler
    raise(SIGSTOP);

    // Initial synchronization
    printf(YELLOW "PROCESS (PID %d): Started Running at %d " RESET "\n", getpid(), get_clk());

    while (runtime > 0)
    {
        if (get_clk() != last_clk)
        {
            runtime--;
            last_clk = get_clk();
            if (runtime <= 0)
                kill(sch_pid, SIGUSR1);
            else
                kill(sch_pid, SIGUSR2);
        }
        usleep(10000);
    }

    printf(YELLOW "PROCESS (PID %d): Process %d finished execution at time %d" RESET "\n", getpid(), getpid(), get_clk());

    destroy_clk(0);
    exit(EXIT_SUCCESS);
}