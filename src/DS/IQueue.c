#include "IQueue.h"

void init_queue(IQueue* queue, SchedulingAlgorithm algType) {
    QueueType type;
    switch (algType) {
        case RR:
            type = CIRCULAR_QUEUE;
            break;
        case HPF:
        case SRTN:
            type = PRIORITY_QUEUE;
            break;
        default:
            type = CIRCULAR_QUEUE; // Default to circular queue
            break;
    }
    queue->type = type;
    switch (type) {
        case CIRCULAR_QUEUE:
            queue->q = circular_queue();
            break;
        case PRIORITY_QUEUE:
            queue->q = priority_queue();
            break;
    }
}

void enqueue(IQueue* queue, void* obj, int priority) {
    switch (queue->type) {
        case CIRCULAR_QUEUE:
            cq_enqueue((CircularQueue*)queue->q, obj);
            break;
        case PRIORITY_QUEUE:
            pq_enqueue((PriorityQueue*)queue->q, obj, priority);
            break;
        default:
            break;
    }
}

void* dequeue(IQueue* queue, int remove) {
    switch (queue->type) {
        case CIRCULAR_QUEUE:
            if (remove) {
                return cq_dequeue_and_remove((CircularQueue*)queue->q);
            }
            else {
                return cq_dequeue((CircularQueue*)queue->q);
            }
        case PRIORITY_QUEUE:
            return pq_dequeue((PriorityQueue*)queue->q);
        default:
            return NULL;
    }
}

void* front(IQueue* queue) {
    switch (queue->type) {
        case CIRCULAR_QUEUE:
            return cq_front((CircularQueue*)queue->q);
        case PRIORITY_QUEUE:
            return pq_front((PriorityQueue*)queue->q);
        default:
            return NULL;
    }
}

int is_empty(IQueue* queue) {
    switch (queue->type) {
        case CIRCULAR_QUEUE:
            return cq_is_empty((CircularQueue*)queue->q);
        case PRIORITY_QUEUE:
            return pq_is_empty((PriorityQueue*)queue->q);
        default:
            return 1;
    }
}

void free_queue(IQueue* queue) {
    switch (queue->type) {
        case CIRCULAR_QUEUE:
            cq_free((CircularQueue*)queue->q);
            break;
        case PRIORITY_QUEUE:
            pq_free((PriorityQueue*)queue->q);
            break;
    }
}

int size(IQueue* queue) {
    switch (queue->type) {
        case CIRCULAR_QUEUE:
            return cq_size((CircularQueue*)queue->q);
        case PRIORITY_QUEUE:
            return pq_size((PriorityQueue*)queue->q);
        default:
            return 0;
    }
}


void change_priority(IQueue* queue, int index, int new_priority) {
    if (queue->type == PRIORITY_QUEUE) {
        pq_change_priority((PriorityQueue*)queue->q, index, new_priority);
    }
}