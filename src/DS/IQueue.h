#ifndef QUEUE_INTERFACE_H
#define QUEUE_INTERFACE_H
#include "circular_queue.h"
#include "priority_queue.h"
#include "../Algorithms/utils.h"

typedef enum QueueType { CIRCULAR_QUEUE, PRIORITY_QUEUE } QueueType;

typedef struct IQueue
{
    void* q;
    QueueType type;
} IQueue;

void init_queue(IQueue* queue, SchedulingAlgorithm algType);
void enqueue(IQueue* queue, void* obj, int priority);
void* dequeue(IQueue* queue, int remove);
void* front(IQueue* queue);
int is_empty(IQueue* queue);
void free_queue(IQueue* queue);
int size(IQueue* queue);
void change_priority(IQueue* queue, int index, int new_priority);


#endif /* QUEUE_INTERFACE_H */