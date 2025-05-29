#include "memory_manager.h"

// Define a global waiting list for PCBs
Block *memory = NULL;

HashMap *memory_map = NULL; // HashMap to store memory blocks

void init_memory_manager()
{
    memory = init_memory();
    init_memory_log();
    memory_map = hashmap_create(100);
}

int deallocate_process_memory(int pid)
{
    ProcessMemory *pmem = hashmap_get(memory_map, pid);
    if (pmem == NULL || pmem->memory == NULL)
    {
        printf("Error: No memory block found for process %d\n", pid);
        return 0;
    }
    log_memory_event(pmem->id, pmem->memory, 0); // Log deallocation event
    deallocate_memory(pmem->memory);
    // print_memo(memory);
    return 1; // Deallocation successful
}

Block *allocate_process_memory(int size)
{
    Block *allocated = allocate_memory(memory, size);
    return allocated; // either null no space or return the address of the allocated memory set it at generator
}

void assign_memory_block_to_process(PCB *process, Block *block)
{
    if (block == NULL)
    {
        printf("Error: No memory block available for process %d\n", process->PDATA.id);
        return;
    }
    ProcessMemory *process_memory = malloc(sizeof(ProcessMemory));
    process_memory->memory = block;
    process_memory->id = process->PDATA.id;
    if (hashmap_put(memory_map, process->pid, process_memory) > 0)
    {
        process->memory_block = block;
        log_memory_event(process_memory->id, block, 1); // Log allocation event
    }
    else
    {
        printf("Error: Failed to assign memory block to process %d\n", process->pid);
        return;
    }
}

// void add_to_waiting_list(PCB* process)
// {
//     Node* node = create_node(process);
//     add_node_to_back(waiting_list, node);
// }

// TODO GET THE BLOCK TOO
Process *get_oldest_fit(LinkedList *waiting_list) // search for the oldest process that can fit and return it after removing it from wiaitng list
{
    if (waiting_list->size == 0)
    {
        return NULL;
    }


    Node *current = waiting_list->head;
    while (current != NULL)
    {
        Process *process = (Process *)current->data;
        Block *fit_block = get_smallest_block_available(memory, process->memory_size);
        if (fit_block != NULL)
        {
            printf("Debug: Found fitting process %d with memory size %d, start %d, allc %d\n", process->id, fit_block->size, fit_block->start, fit_block->allocated);
            remove_node(waiting_list, current);
            return process; // Found a process that fits
        }
        current = current->next;
    }

    return NULL; // No fitting process found
}

Block* get_smallest_block(Process* process)
{
    Block* block = get_smallest_block_available(memory, process->memory_size);
    if (block == NULL)
    {
        printf("Error: No memory block available for process %d\n", process->id);
        return NULL;
    }
    return block;
}

void free_memory_manager()
{
    destroy_memory(memory);
    hashmap_free(memory_map);
}