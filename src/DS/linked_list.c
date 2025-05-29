#include <stdio.h>
#include <stdlib.h>
#include "linked_list.h"
#include "../PCB.h"

Node *create_node(void *obj)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->data = obj;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void add_node_to_back(LinkedList *listPtr, Node *nodePtr)
{
    PCB * PPP = (PCB* ) nodePtr->data;
    printf("LL : Process (PID: %d) IAM HERE" "\n" , PPP->pid);

    listPtr->size++;
    if (listPtr->head == NULL)
    {
        listPtr->head = nodePtr;
        listPtr->tail = nodePtr;
    }
    else
    {
        listPtr->tail->next = nodePtr;
        nodePtr->prev = listPtr->tail;
        listPtr->tail = nodePtr;
    }
}

void add_node_to_front(LinkedList *listPtr, Node *nodePtr)
{
    listPtr->size++;
    if (listPtr->head == NULL)
    {
        listPtr->head = nodePtr;
        listPtr->tail = nodePtr;
    }
    else
    {
        listPtr->head->prev = nodePtr;
        nodePtr->next = listPtr->head;
        listPtr->head = nodePtr;
    }
}

LinkedList *create_linked_list()
{
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void *remove_node_from_front(LinkedList *listPtr)
{
    listPtr->size--;
    if (listPtr->head == NULL)
    {
        return NULL;
    }
    else if (listPtr->head == listPtr->tail)
    {
        listPtr->head = NULL;
        listPtr->tail = NULL;
    }
    else
    {
        Node *temp = listPtr->head;
        listPtr->head = listPtr->head->next;
        listPtr->head->prev = NULL;
    }
}

void *remove_node_from_back(LinkedList *listPtr)
{
    listPtr->size--;
    if (listPtr->head == NULL)
    {
        return NULL;
    }
    else if (listPtr->head == listPtr->tail)
    {
        listPtr->head = NULL;
        listPtr->tail = NULL;
    }
    else
    {
        Node *temp = listPtr->tail;
        listPtr->tail = listPtr->tail->prev;
        listPtr->tail->next = NULL;
    }
}

void *remove_node(LinkedList *listPtr, Node *nodePtr)
{
    Node* temp = nodePtr;
    if (nodePtr == listPtr->head)
    {
        remove_node_from_front(listPtr);
    }
    else if (nodePtr == listPtr->tail)
    {
        remove_node_from_back(listPtr);
    }
    else
    {
        listPtr->size--;
        nodePtr->prev->next = nodePtr->next;
        nodePtr->next->prev = nodePtr->prev;
    }
    free(temp);
}

void *peek(LinkedList *listPtr) 
{
    if (listPtr == NULL || listPtr->head == NULL) {
        return NULL;
    }
    return listPtr->head->data;
}

void free_linked_list(LinkedList *listPtr)
{
    if (listPtr == NULL) {
        return;
    }

    Node *current = listPtr->head;
    while (current != NULL) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }

    free(listPtr);
}