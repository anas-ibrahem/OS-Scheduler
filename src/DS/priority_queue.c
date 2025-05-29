#include "priority_queue.h"
#include <stdio.h>
#include "../process.h"

/**
 * @brief Create a new priority queue
 * @param[in] capacity maximum size of priority queue
 * @return CircularQueue* Pointer to the newly created PriorityQueue
 */

PriorityQueue* priority_queue() {
    // start with init capacity 
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));

    pq->size = 0;
    // init capacity
    pq->capacity = 100;

    pq->nodes = (PQueueNode**)malloc(pq->capacity * sizeof(PQueueNode*));
    return pq;
}

/**
 * @brief Enqueue an item into the priority queue
 * @param[in] pq Pointer to the PriorityQueue structure
 * @param[in] item Pointer to the item itself to enqueue
 * @return void
 */
void pq_enqueue(PriorityQueue* pq, void* item, int priority) {
    // create new node
    PQueueNode* newNode = (PQueueNode*)malloc(sizeof(PQueueNode));
    newNode->item = item;

    // invert priority to make it min heap
    newNode->priority = -priority;

    // check capacity first 
    if (pq->size == pq->capacity) {
        printf("size %d capacity: %d\n", pq->size, pq->capacity);

        // double the init capacity
        pq->capacity = pq->capacity * 2;

        // re-allocate
        pq->nodes = realloc(pq->nodes, pq->capacity * sizeof(PQueueNode*));

        if (pq->nodes == NULL) {
            fprintf(stderr, "Error: Failed to realloc memory for priority queue.\n");
        }
    }

    printf("process %d added to queue\n", ((Process*)item)->id);
    // insert new item
    pq->nodes[pq->size] = newNode;
    pq->size++;

    // update heap
    int parent = pq->size - 1;

    while (parent > 0) {
        int child = (parent - 1) / 2;

        if (compare_priority(pq->nodes[parent], pq->nodes[child]) > 0) {
            // swap
            pq_swap(&pq->nodes[parent], &pq->nodes[child]);
            parent = child;
        }
        else {
            break;
        }
    }

    printf("top process: %d\n", ((Process*)pq->nodes[0]->item)->id);
}


/**
 * @brief Dequeue an item from the priority queue
 * @param[in] pq Pointer to the PriorityQueue structure
 * @return void* Pointer to the dequeued item
 */
void* pq_dequeue(PriorityQueue* pq) {
    if (pq_is_empty(pq)) return NULL;

    void* frontItem = pq->nodes[0]->item;

    int n = pq->size;
    pq_swap(&pq->nodes[0], &pq->nodes[n - 1]);

    free(pq->nodes[n - 1]);
    pq->nodes[n - 1] = NULL;

    pq->size--;

    if(!pq_is_empty(pq)) {
        pq_heapify(pq, 0);
    }

    // pq_heapify(pq, 0);

    return frontItem;
}

/**
 * @brief Get the object at the front of the priority queue
 * @param[in] pq Pointer to the PriorityQueue structure
 * @return void* Pointer to the item at the front of the queue, or NULL if the queue is empty
 */
void* pq_front(PriorityQueue* pq) {
    if (pq_is_empty(pq)) return NULL;

    return pq->nodes[0]->item;
}

/**
 * @brief Compare the priority of two nodes
 * @param[in] a Pointer to the first node
 * @param[in] b Pointer to the second node
 * @return Positive if item1 > item2, negative if item1 < item2, zero if equal
 */
int compare_priority(PQueueNode* a, PQueueNode* b) {
    return a->priority - b->priority;
}

/**
 * @brief Get the number of elements in the priority queue
 * @param pq Pointer to the priority queue
 * @return The size of the priority queue
 */
int pq_size(PriorityQueue* pq) {
    return pq->size;
}

/**
 * @brief Check if the priority queue is empty
 * @param[in] pq Pointer to the PriorityQueue
 * @return true if priority queue is empty, false otherwise
 */
bool pq_is_empty(PriorityQueue* pq) {
    return pq->size == 0;
}

/**
 * @brief Swap two nodes in the priority queue
 * @param[in] a First node
 * @param[in] b Second node
 */

void pq_swap(PQueueNode** a, PQueueNode** b) {
    PQueueNode* temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Maintian the heap property
 * @param[in] pq Pointer to the priority queue
 * @param[in] parent Index of the parent node to heapify from
 */
void pq_heapify(PriorityQueue* pq, int parent) {
    int smallest = parent;
    int left = 2 * parent + 1;
    int right = 2 * parent + 2;

    int n = pq->size;

    // If left node is smaller than parent node
    if (left < n && compare_priority(pq->nodes[left], pq->nodes[smallest]) > 0)
        smallest = left;

    // If right node is smaller than smallest so far
    if (right < n && compare_priority(pq->nodes[right], pq->nodes[smallest]) > 0)
        smallest = right;

    // If smallest is not parent node
    if (smallest != parent) {
        pq_swap(&pq->nodes[parent], &pq->nodes[smallest]);
        pq_heapify(pq, smallest);
    }
}

void pq_free(PriorityQueue* pq) {
    if (pq_is_empty(pq)) {
        free(pq);
        pq = NULL;
        return;
    }

    // free each node
    for (int i = 0; i < pq->size; i++) {
        free(pq->nodes[i]);
        pq->nodes[i] = NULL;
    }

    // Free the array after
    free(pq->nodes);
    pq->nodes = NULL;

    free(pq);
    pq = NULL;
}

/**
 * @brief Change the priority of an item at a specific index in the priority queue
 * @param[in] pq Pointer to the PriorityQueue structure
 * @param[in] index Index of the item whose priority needs to be changed
 * @param[in] new_priority New priority value to set
 * @return void
 */
void pq_change_priority(PriorityQueue* pq, int index, int new_priority) {
    // Check if index is valid
    if (index < 0 || index >= pq->size) {
        fprintf(stderr, "Error: Index out of bounds for priority queue.\n");
        return;
    }
    
    // Get the old priority
    int old_priority = pq->nodes[index]->priority;
    
    // Update to the new priority (inverted to maintain min-heap property)
    pq->nodes[index]->priority = -new_priority;
    
    // If new priority is higher (which means the inverted value is lower),
    // move the node up in the heap
    if (-new_priority > old_priority) {
        // Move up
        int parent = index;
        while (parent > 0) {
            int child = (parent - 1) / 2;
            
            if (compare_priority(pq->nodes[parent], pq->nodes[child]) > 0) {
                // swap
                pq_swap(&pq->nodes[parent], &pq->nodes[child]);
                parent = child;
            } else {
                break;
            }
        }
    }
    // If new priority is lower (which means the inverted value is higher),
    // move the node down in the heap
    else if (-new_priority < old_priority) {
        // Move down by heapifying
        pq_heapify(pq, index);
    }
    // If same priority, nothing to do
}