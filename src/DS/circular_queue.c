#include "circular_queue.h"

/**
 * @brief Create a new circular queue.
 * 
 * This function initializes a new circular queue and returns a pointer to it.
 * 
 * @return CircularQueue* Pointer to the newly created CircularQueue structure.
 */
CircularQueue* circular_queue() {
    CircularQueue* queue = (CircularQueue*)malloc(sizeof(CircularQueue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    return queue;
}

/**
 * @brief Enqueue an object into the circular queue.
 * 
 * This function adds an object to the end of the circular queue.
 * 
 * @param[in] q Pointer to the CircularQueue structure.
 * @param[in] obj Pointer to the object to be added to the queue.
 * @return void
 */
void cq_enqueue(CircularQueue* q, void* obj) {
    QueueNode* node = (QueueNode*)malloc(sizeof(QueueNode));
    node->obj = obj;
    if(q->tail == NULL) { // queue is empty, create first node
        q->head = q->tail = node;
        node->next = node;
        node->prev = node;
    } else { // queue is not empty, add to the end
        node->next = q->tail->next; // link new node to head
        node->prev = q->tail; // link new node to the previous tail
        q->tail->next->prev = node; // link head's prev to new node
        q->tail->next = node; // link tail's next to new node
        q->tail = node; // update tail to new node
    }
    q->size++;
}

/**
 * @brief Dequeue an object from the circular queue.
 * 
 * This function removes the head object from the queue and returns it.
 * 
 * @param[in] q Pointer to the CircularQueue structure.
 * @return void* Pointer to the dequeued object, or NULL if the queue is empty.
 */
void* cq_dequeue(CircularQueue* q) {
    if(q->head == NULL) { // queue is empty
        return NULL;
    }
    QueueNode* node = q->head;
    void* obj = node->obj;
    q->head = node->next;
    return obj;
}

/**
 * @brief Dequeue and remove an object from the circular queue.
 * 
 * This function removes the head object from the queue and frees its memory.
 * 
 * @param[in] q Pointer to the CircularQueue structure.
 * @return void* Pointer to the dequeued object, or NULL if the queue is empty.
 */
void* cq_dequeue_and_remove(CircularQueue* q) {
    if(q->head == NULL) { // queue is empty
        return NULL;
    }
    QueueNode* node = q->head;
    void* obj = node->obj;
    if(node == q->tail && node == q->tail->next) { // only one node in the queue
        free(node);
        q->tail = q->head = NULL;
    } else {
        node->prev->next = node->next; // link previous to next
        node->next->prev = node->prev; // link next to previous
        if(node == q->tail) { // if tail is being removed
            q->tail = node->prev; // update tail to previous
        }
        
        q->head = node->next; // update head to next
        
        free(node);
    }
    q->size--;

    return obj;
}

/**
 * @brief Get the object at the front of the circular queue.
 * 
 * This function returns the object at the front of the queue without removing it.
 * 
 * @param[in] q Pointer to the CircularQueue structure.
 * @return void* Pointer to the object at the front of the queue, or NULL if the queue is empty.
 */
void* cq_front(CircularQueue* q) {
    if(q->head == NULL) { // queue is empty
        return NULL;
    }
    return q->head->obj;
}

/**
 * @brief Free the memory allocated for the circular queue.
 * 
 * This function frees the memory allocated for the circular queue and its nodes.
 * 
 * @param[in] q Pointer to the CircularQueue structure to be freed.
 * @return void
 */
void cq_free(CircularQueue* q) {
    if (q == NULL || q->tail == NULL) {
        free(q);
        return;
    }
    QueueNode* current = q->tail->next; // Start from the head
    q->tail->next = NULL; // Break circular link
    while (current != NULL) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(q);
}

/**
 * @brief Check if the circular queue is empty.
 * 
 * This function checks if the circular queue is empty.
 * 
 * @param[in] q Pointer to the CircularQueue structure.
 * @return int 1 if the queue is empty, 0 otherwise.
 */
int cq_is_empty(CircularQueue* q) {
    return q->size == 0;
}

/**
 * @brief Get the size of the circular queue.
 * 
 * This function returns the number of elements in the circular queue.
 * 
 * @param[in] q Pointer to the CircularQueue structure.
 * @return int The size of the queue.
 */
int cq_size(CircularQueue* q) {
    return q->size;
}