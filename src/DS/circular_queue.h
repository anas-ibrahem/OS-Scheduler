#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdlib.h>

// Structure definitions
typedef struct QueueNode {
    void* obj;
    struct QueueNode* next;
    struct QueueNode* prev;
} QueueNode;

typedef struct {
    QueueNode* head;
    QueueNode* tail;
    int size;
} CircularQueue;

// Functions
CircularQueue* circular_queue();
void cq_enqueue(CircularQueue* q, void* obj);
void* cq_dequeue(CircularQueue* q);
void* cq_dequeue_and_remove(CircularQueue* q);
void* cq_front(CircularQueue* q);
void cq_free(CircularQueue* q);
int cq_is_empty(CircularQueue* q);
int cq_size(CircularQueue* q);

#endif /* CIRCULAR_QUEUE_H */
