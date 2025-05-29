#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "buddy_memory.h"

Block *memo;
/**
 * @brief Initialize memory structure.
 *
 * @return Block* Pointer to memory block.
 */
Block *init_memory()
{
    Block *memory = (Block *)malloc(sizeof(Block));
    memory->size = MAX_SIZE;
    memory->start = 0;
    memory->next = NULL;
    memory->prev = NULL;
    memory->allocated = 0;
    memo = memory;
    return memory;
}

/**
 * @brief Get the smallest available block
 *
 * @param[in] memory pointer to memory
 * @param[in] size size of memory to be allocated
 *
 * @return Block Pointer to the smallest block
 */
Block *get_smallest_block_available(Block *memory, int size)
{
    Block *current = memory;
    Block *smallest = NULL;

    while (current != NULL)
    {
        // Check if not allocated and size is bigger than needed size
        if (current->allocated == 0 && current->size >= size && (smallest == NULL || current->size < smallest->size))
        {
            smallest = current;
        }
        current = current->next;
    }

    return smallest;
}

/**
 * @brief Split block into two
 *
 * @param[in] block pointer to block
 */
void split_block(Block *block)
{
    block->size /= 2; // Split size

    Block *new_block = (Block *)malloc(sizeof(Block));

    // Initialize new block
    new_block->size = block->size;
    new_block->start = block->start + block->size;
    new_block->prev = block;
    new_block->next = block->next;
    new_block->allocated = 0;

    block->next = new_block;

    if (new_block->next != NULL)
    {
        new_block->next->prev = new_block;
    }
}

/**
 * @brief Allocate memory for a given size
 *
 * @param[in] memory pointer to memory
 * @param[in] size size of memory to be allocated
 *
 * @return Block Pointer to the allocated buddy block
 */
Block *allocate_memory(Block *memory, int size)
{
    // Get smallest block available
    Block *block = get_smallest_block_available(memory, size);

    if (block == NULL)
    {
        return NULL;
    }

    int required_order = (int)ceil(log2(size));

    while ((int)(log2(block->size)) != required_order)
    {
        split_block(block);
    }

    block->allocated = size;

    return block;
}

/**
 * @brief Merge blocks
 *
 * @param[in] block pointer to block
 */
void merge_block(Block *block)
{
    int buddy_start = block->start ^ block->size;

    // Buddy is on the right
    if (buddy_start > block->start)
    {
        Block *buddy = block->next;      

        if (buddy != NULL && buddy->size == block->size && buddy->allocated == 0)
        {
            block->size *= 2;
            block->next = buddy->next;
            if (buddy->next != NULL)
            {
                buddy->next->prev = block;
            }
                print_memo(memo);
            free(buddy);
            merge_block(block);
        }
    }

    // Buddy is on the left
    else
    {
        Block *buddy = block->prev;
        if (buddy != NULL && buddy->size == block->size && buddy->allocated == 0)
        {
            buddy->size *= 2;
            buddy->next = block->next;
            if (block->next != NULL)
            {
                block->next->prev = buddy;
            }
        
            free(block);
            merge_block(buddy);
        }
    }
}

/**
 * @brief Deallocate memory
 *
 * @param[in] block pointer to block to be deallocated
 */
void deallocate_memory(Block *block)
{
    // Check if block exists
    if (block == NULL)
    {
        printf("Deallocating failed: no such a block!\n");
        return;
    }

    block->allocated = 0;
    merge_block(block);
}

void destroy_memory(Block *memory)
{
    Block *current = memory;
    while (current != NULL)
    {
        Block *temp = current;
        current = current->next;
        free(temp);
    }
}

void print_memo(Block *memory)
{
    Block *current = memory;
    while (current != NULL)
    {
        printf("start: %d, size: %d, allocated: %d\n", current->start, current->size, current->allocated);
        current = current->next;
    }
    printf("Done printing memory\n");
}
