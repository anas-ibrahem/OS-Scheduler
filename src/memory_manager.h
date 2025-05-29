#include "PCB.h"
#include <stddef.h>
#include "buddy_memory.h"
#include "DS/linked_list.h"
#include "DS/hash_map.h"
#include "file_handlers.h"
#include <stdio.h>

typedef struct {
    Block *memory;
    int id; // process id
} ProcessMemory;

void init_memory_manager();
void free_memory_manager();
Block *allocate_process_memory (int size);
int deallocate_process_memory (int pid);
Process* get_oldest_fit(LinkedList * waiting_list);
Block* get_smallest_block(Process* process);
void assign_memory_block_to_process(PCB* process, Block* block);
// search for the oldest process that can fit and return it after removing it from wiaitng list
