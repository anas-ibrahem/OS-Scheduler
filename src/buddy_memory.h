#ifndef BUDDY_MEMORY_H
#define BUDDY_MEMORY_H

#define MAX_SIZE 1024

typedef struct Block {
    int size;
    int start;
    int allocated;
    struct Block *next;
    struct Block *prev;
} Block;

Block *init_memory();
Block *get_smallest_block_available(Block *memory, int size);
void split_block(Block *block);
Block *allocate_memory(Block *memory, int size);
void merge_block(Block *block);
void deallocate_memory(Block *block);
void destroy_memory(Block *memory);
void print_memo(Block *memory);

#endif //BUDDY_MEMORY_H