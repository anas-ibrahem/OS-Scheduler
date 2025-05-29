#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "clk.h"

#include "file_handlers.h"
#include "PCB.h"

FILE* scheduler_log = NULL; // File to log execution events
double total_WTA = 0;

FILE* scheduler_perf = NULL; // File to log execution events
int total_wait = 0;
int proc_count;
double* WTAs;

FILE* memory_log = NULL; // File to log memory events

// exit code 0 for success, 1 for error
int read_input_file(const char* filename, Process** process_list, int* process_count) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    *process_list = NULL;
    *process_count = 0;

    while (1) {
        char line[256];
        if (fgets(line, sizeof(line), file) == NULL)
            break;

        // Ignore comment lines and empty lines
        if (line[0] == '#' || line[0] == '\n')
            continue;

        // Re-alloc the array memroy for the new process
        Process* temp = realloc(*process_list, (*process_count + 1) * sizeof(Process));

        if (temp == NULL) {
            perror("Error reallocating memory");
            free(*process_list);
            fclose(file);
            return 1;
        }

        // Update array size
        *process_list = temp;

        // Use sscanf to parse tab-separated values
        if (sscanf(line, "%d\t%d\t%d\t%d\t%d",
            &(*process_list)[*process_count].id,
            &(*process_list)[*process_count].arrival_time,
            &(*process_list)[*process_count].runtime,
            &(*process_list)[*process_count].priority,
            &(*process_list)[*process_count].memory_size) != 5) {

            fprintf(stderr, "Error parsing line: %s", line);
            free(*process_list);
            fclose(file);
            return 1;
        }

        // debug line
        // printf("Read process: ID=%d, Arrival=%d, Runtime=%d, Priority=%d, Memory=%d\n",
        //     (*process_list)[*process_count].id,
        //     (*process_list)[*process_count].arrival_time,
        //     (*process_list)[*process_count].runtime,
        //     (*process_list)[*process_count].priority,
        //     (*process_list)[*process_count].memory_size);

        // Set default values for newly created process
        // (*process_list)[*process_count].remaining_time = (*process_list)[*process_count].runtime;
        // (*process_list)[*process_count].start_time = -1;
        // (*process_list)[*process_count].wait_time = 0;

        (*process_count)++;
    }

    // update processes count
    proc_count = *process_count;

    fclose(file);
    return 0;
}

/**
 * @brief Initialize scheduler.log file and print the header
 */
void init_scheduler_log() {
    WTAs = (double*)malloc(proc_count * sizeof(double));

    scheduler_log = fopen("scheduler.log", "w");
    if (scheduler_log == NULL) {
        perror("Failed to create scheduler log file");
        exit(EXIT_FAILURE);
    }

    fprintf(scheduler_log, "At time x process y state arr w total z remain y wait k\n");
    fflush(scheduler_log);
}

void init_memory_log() {
    memory_log = fopen("memory.log", "w");
    if (memory_log == NULL) {
        perror("Failed to create memory log file");
        exit(EXIT_FAILURE);
    }

    fprintf(memory_log, "#At time x allocated y bytes for process z from i to j\n");
    fflush(memory_log);
}
/**
 * @brief Log event into scheduler.log
 *
 * @param[in] process* pointer to process to be logged
 */
void log_event(PCB* pcb) {
    if (pcb == NULL) {
        printf("Error: Null process pointer\n");
        return;
    }


    int current_time = get_clk();

    Process process = pcb->PDATA;
    if (strcmp(pcb->state, "finished") == 0) {

        int TA = current_time - process.arrival_time; // Calculate TA
        double WTA = ((double)TA) / process.runtime;  // Calculate WTA
        total_WTA += WTA;                         // Add WTA total WTA
        total_wait += pcb->wait_time;

        char* wta_str = round_number(WTA); // Round and remove trailing zeros
        fprintf(scheduler_log, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WTA %s\n",
            current_time,
            process.id,
            pcb->state,
            process.arrival_time,
            process.runtime,
            pcb->remaining_time,
            pcb->wait_time,
            TA,
            wta_str);
    }
    else {
        fprintf(scheduler_log, "At time %d process %d %s arr %d total %d remain %d wait %d\n",
            current_time,
            process.id,
            pcb->state,
            process.arrival_time,
            process.runtime,
            pcb->remaining_time,
            pcb->wait_time);
    }
    fflush(scheduler_log);
}

void log_memory_event(int pid, Block* block, int type) {
    if (block == NULL) {
        printf("Error: Null process or block pointer\n");
        return;
    }

    int current_time = get_clk();

    if(type == 0) {
        // Deallocation
        fprintf(memory_log, "At time %d deallocated %d bytes for process %d from %d to %d\n",
            current_time,
            block->allocated,
            pid,
            block->start,
            block->start + block->size - 1);
    }
    else if (type == 1) {
        // Allocation
        fprintf(memory_log, "At time %d allocated %d bytes for process %d from %d to %d\n",
            current_time,
            block->allocated,
            pid,
            block->start,
            block->start + block->size - 1);
    }
    else {
        fprintf(stderr, "Error: Invalid memory event type\n");
        return;
    }
    
    fflush(memory_log);
}
/**
 * @brief Calculate performance fields and print in scheduler.perf
 *
 * @param[in] int total_idle, int total_time
 */
void scheduler_perf_out(int total_idle, int total_time) {
    double avg_wta = total_WTA / proc_count;             // Average WTA
    double avg_wait = ((double)total_wait) / proc_count; // Average WTA

    double diff_square_sum = 0; // Variable to sum the square of differences for the std of WTA

    for (int i = 0; i < proc_count; i++) {
        double diff = WTAs[i] - avg_wta;
        diff_square_sum += diff * diff;
    }

    double std_wta = sqrt(diff_square_sum / proc_count);

    double cpu_utilization = (1 - (((double)total_idle) / total_time)) * 100;

    // Round and trim trailing zeros for all numbers
    char* cpu_utilization_str = round_number(cpu_utilization);
    char* avg_wta_str = round_number(avg_wta);
    char* avg_wait_str = round_number(avg_wait);
    char* std_wta_str = round_number(std_wta);

    // Open scheduler.perf file
    scheduler_perf = fopen("scheduler.perf", "w");
    if (scheduler_perf == NULL) {
        perror("Failed to create scheduler perf file");
        exit(EXIT_FAILURE);
    }

    // Print to scheduler.perf file
    fprintf(scheduler_perf, "CPU utilization = %s%\n", cpu_utilization_str);
    fprintf(scheduler_perf, "Avg WTA = %s\n", avg_wta_str);
    fprintf(scheduler_perf, "Avg Waiting = %s\n", avg_wait_str);
    fprintf(scheduler_perf, "Std WTA = %s\n", std_wta_str);
    fflush(scheduler_perf);

    free(cpu_utilization_str);
    free(avg_wta_str);
    free(avg_wait_str);
    free(std_wta_str);
    free(WTAs);
}

/**
 * @brief Trim trailing zeros of a double
 *
 * @param[in] double number to be rounded
 *
 * @return char* trimmed string
 */
char* round_number(double number) {
    // Format WTA to 2 decimal places and trim trailing zeros
    char* str = malloc(32);                     // Allocate memory
    snprintf(str, sizeof(str), "%.2f", number); // Round and format to string
    int len = strlen(str);
    while (len > 0 && str[len - 1] == '0') {
        str[--len] = '\0'; // Remove trailing zero
    }
    if (len > 0 && str[len - 1] == '.') {
        str[--len] = '\0'; // Remove trailing decimal point if present
    }
    return str;
}