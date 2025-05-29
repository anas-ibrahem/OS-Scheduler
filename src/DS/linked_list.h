#ifndef LINKED_LIST
#define LINKED_LIST

typedef struct node
{
    struct node *next;
    struct node *prev;
    void *data;
} Node;

typedef struct linked_list
{
    Node *head;
    Node *tail;
    int size;
} LinkedList;

/**
 * @brief creates a linked list node
 * @param obj the object to add reference to in the linked list
 * @return pointer to linked list node created
 */
Node *create_node(void *obj);

/**
 * @brief creates a linked list
 * @return pointer to linked list created
 */
LinkedList *create_linked_list();

/**
 * @brief adds node to linked list
 * @param listPtr pointer to linked list
 * @param nodePtr pointer to node
 * @return void
 */
void add_node(LinkedList *listPtr, Node *nodePtr);

/**
 * @brief adds node to end of linked list
 * @param  listPtr pointer to linked list
 * @param  nodePtr pointer to node
 * @return void
 */
void add_node_to_back(LinkedList *listPtr, Node *nodePtr);

/**
 * @brief adds node to front of a linked list
 * @param listPtr pointer to linked list
 * @param nodePtr pointer to node
 * @return void
 */
void add_node_to_front(LinkedList *listPtr, Node *nodePtr);

/**
 * @brief removes node from linked list
 * @param listPtr pointer to linked list
 * @param nodePtr pointer to node
 * @return pointer to node
 */
void *remove_node(LinkedList *listPtr, Node *nodePtr);

/**
 * @brief removes node from front of linked list
 * @param listPtr pointer to linked list
 * @return pointer to node
 */
void *remove_node_from_front(LinkedList *listPtr);

/**
 * @brief removes node from back of linked list
 * @param listPtr pointer to linked list
 * @return pointer to node
 */
void *remove_node_from_back(LinkedList *listPtr);

/**
 * @brief peeks at the first node of the linked list
 * @param listPtr pointer to linked list
 * @return pointer to data of the first node, or NULL if the list is empty
 */
void *peek(LinkedList *listPtr);

/**
 * @brief Frees the memory allocated for the linked list and its nodes.
 * @param listPtr pointer to the linked list to be freed
 */
void free_linked_list(LinkedList *listPtr);

#endif