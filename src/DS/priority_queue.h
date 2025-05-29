#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    void* item;
    int priority;
} PQueueNode;

typedef struct {
    int size; // actual size
    int capacity;// allocated memory
    PQueueNode** nodes; // array of nodes
} PriorityQueue;

PriorityQueue* priority_queue();
void pq_enqueue(PriorityQueue* pq, void* item, int priority);
void* pq_dequeue(PriorityQueue* pq);
void* pq_front(PriorityQueue* pq);

int pq_size(PriorityQueue* pq);
bool pq_is_empty(PriorityQueue* pq);

int compare_priority(PQueueNode* a, PQueueNode* b);
void pq_swap(PQueueNode** a, PQueueNode** b);
void pq_heapify(PriorityQueue* pq, int i);
void pq_change_priority(PriorityQueue* pq, int index, int new_priority);

void pq_free(PriorityQueue* pq);

#endif