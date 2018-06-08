#include <stdlib.h>
#include <stdio.h>
#include "doublely_linked_list.h"

DLinkedList* create_dlinkedlist(void) {
    DLinkedList* newList = (DLinkedList*)malloc(sizeof(DLinkedList));
    newList->head = NULL;
    newList->tail = NULL;
    newList->current = NULL;
    newList->size = 0;
    return newList;
}

LLNode* create_llnode(void* data) {
    LLNode* newNode = (LLNode*)malloc(sizeof(LLNode));
    newNode->data = data;
    newNode->previous = NULL;
    newNode->next = NULL;
    return newNode;
}

void insertHead(DLinkedList* dLinkedList, void* data) {
    LLNode* newNode = create_llnode(data);

    if (dLinkedList->size == 0) {
        dLinkedList->head = newNode;
        dLinkedList->tail = newNode;
        dLinkedList->size = 1;
    } else {
        newNode->next = dLinkedList->head;          // node->head
        (dLinkedList->head)->previous = newNode;    // node<-head
        dLinkedList->head = newNode;                // head->node
        dLinkedList->size++;
    }
}

void insertTail(DLinkedList* dLinkedList, void* data) {
    LLNode* newNode = create_llnode(data);          // create node
    if (dLinkedList->head == NULL) {                // case empty
        dLinkedList->size++;
        dLinkedList->head = newNode;
        dLinkedList->tail = newNode;
    } else {                                        // regular case
        dLinkedList->size++;                        // incr size
        newNode->previous = dLinkedList->tail;      // set new prev = tail
        (dLinkedList->tail)->next = newNode;        // set tail's next to new
        dLinkedList->tail = newNode;                // set tail = new
    }
}

int insertAfter(DLinkedList* dLinkedList, void* newData) {
    if (dLinkedList->current == NULL) {                     // curr is null
        return 0;
    } else if (dLinkedList->current == dLinkedList->tail) { // curr is tail
        LLNode* newNode = create_llnode(newData);
        newNode->previous = dLinkedList->tail;
        (dLinkedList->tail)->next = newNode;
        dLinkedList->tail = newNode;  
    } else {                                            // normal case
        LLNode* newNode = create_llnode(newData);       // create node
        newNode->previous = dLinkedList->current;
        newNode->next = (dLinkedList->current)->next; 
        ((dLinkedList->current)->next)->previous = newNode;
        (dLinkedList->current)->next = newNode;
    }
    dLinkedList->size++;
    return 1;
}

int insertBefore(DLinkedList* dLinkedList, void* newData) {
    if (dLinkedList->current == NULL) {
        return 0;
    } else if (dLinkedList->current == dLinkedList->head) {
        LLNode* newNode = create_llnode(newData);          // create node
        newNode->next = dLinkedList->head;
        (dLinkedList->head)->previous = newNode;
        dLinkedList->head = newNode;
    } else {
        LLNode* newNode = create_llnode(newData);          // create node
        newNode->previous = (dLinkedList->current)->previous;
        newNode->next = dLinkedList->current;
        ((dLinkedList->current)->previous)->next = newNode;
        (dLinkedList->current)->previous = newNode;
    }
    dLinkedList->size++;
    return 1;
}

void* deleteBackward(DLinkedList* dLinkedList) {
    if (dLinkedList->size == 0) {
        return NULL;
    } else if (dLinkedList->size == 1) {
        // ADJUST POINTERS
        dLinkedList->head = NULL;
        dLinkedList->tail = NULL;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        dLinkedList->current = NULL;
        // UPDATE CURR AND SIZE
        dLinkedList->size = 0;
        return NULL;
    } else if (dLinkedList->current == dLinkedList->head) {
        // ADJUST POINTERS
        dLinkedList->head = (dLinkedList->head)->next;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = NULL;
        dLinkedList->size--;
        return NULL;
    } else if (dLinkedList->current == dLinkedList->tail) {
        dLinkedList->tail = (dLinkedList->tail)->previous;
        (dLinkedList->tail)->next = NULL;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->size--;
        dLinkedList->current = dLinkedList->tail;
        return (dLinkedList->current)->data;
    } else {
        LLNode* newCurr = (dLinkedList->current)->previous;
        // ADJUST POINTERS
        ((dLinkedList->current)->previous)->next = (dLinkedList->current)->next;
        ((dLinkedList->current)->next)->previous = (dLinkedList->current)->previous;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        // UPDATE SIZE AND CURR
        dLinkedList->size--;
        dLinkedList->current = newCurr;
        return (dLinkedList->current)->data;
    }
}

void* deleteForward(DLinkedList* dLinkedList) {
    if (dLinkedList->size == 0) {
        return NULL;
    } else if (dLinkedList->size == 1) {           // size = 1
        // ADJUST POINTERS
        dLinkedList->head = NULL;
        dLinkedList->tail = NULL;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = NULL;
        dLinkedList->size = 0;
        return NULL;
    } else if (dLinkedList->current == dLinkedList->tail) { // tail
        // ADJUST POINTERS
        ((dLinkedList->tail)->previous)->next = NULL;
        dLinkedList->tail = (dLinkedList->tail)->previous;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = NULL;
        dLinkedList->size--;
        return NULL;
    } else if (dLinkedList->current == dLinkedList->head) {
        // ADJUST POINTERS
        dLinkedList->head = (dLinkedList->head)->next;
        (dLinkedList->head)->previous = NULL;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = dLinkedList->head;
        dLinkedList->size--;
        return (dLinkedList->current)->data;
    } else {
        LLNode* newCurr = (dLinkedList->current)->next;
        // ADJUST POINTERS
        ((dLinkedList->current)->previous)->next = (dLinkedList->current)->next;
        ((dLinkedList->current)->next)->previous = (dLinkedList->current)->previous;
        // FREE MEMORY
        free((dLinkedList->current)->data);
        free(dLinkedList->current);
        //UPDATE SIZE AND CURR
        dLinkedList->size--;
        dLinkedList->current = newCurr;
        return (dLinkedList->current)->data;
    }
}

void* removeBackward(DLinkedList* dLinkedList) {
    if (dLinkedList->size == 0) {
        return NULL;
    } else if (dLinkedList->current == NULL) {
        return NULL;
    } else if (dLinkedList->size == 1) {
        // ADJUST POINTERS
        dLinkedList->head = NULL;
        dLinkedList->tail = NULL;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = NULL;
        dLinkedList->size = 0;
        return data;
    } else if (dLinkedList->current == dLinkedList->head) {
        // ADJUST POINTERS
        dLinkedList->head = (dLinkedList->head)->next;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = NULL;
        dLinkedList->size--;
        return data;
    } else if (dLinkedList->current == dLinkedList->tail) {
        // ADJUST POINTERS
        dLinkedList->tail = (dLinkedList->tail)->previous;
        (dLinkedList->tail)->next = NULL;
        // FREE MEMORY
        void* data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->size--;
        dLinkedList->current = dLinkedList->tail;
        return data;
    } else {
        LLNode* newCurr = (dLinkedList->current)->previous;
        // ADJUST POINTERS
        ((dLinkedList->current)->previous)->next = (dLinkedList->current)->next;
        ((dLinkedList->current)->next)->previous = (dLinkedList->current)->previous;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE SIZE AND CURR
        dLinkedList->size--;
        dLinkedList->current = newCurr;
        return data;
    }
}

void* removeForward(DLinkedList* dLinkedList) {
    if (dLinkedList->size == 0) {
        return NULL;
    } else if (dLinkedList->size == 1) {
        // ADJUST POINTERS
        dLinkedList->head = NULL;
        dLinkedList->tail = NULL;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        dLinkedList->current = NULL;
        // UPDATE CURR AND SIZE
        dLinkedList->size = 0;
        return data;
    } else if (dLinkedList->current == dLinkedList->head) {
        // ADJUST POINTERS
        dLinkedList->head = (dLinkedList->head)->next;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->current = (dLinkedList->current)->next;
        dLinkedList->size--;
        return data;
    } else if (dLinkedList->current == dLinkedList->tail) {
        dLinkedList->tail = (dLinkedList->tail)->previous;
        (dLinkedList->tail)->next = NULL;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE CURR AND SIZE
        dLinkedList->size--;
        dLinkedList->current = NULL;
        return data;
    } else {
        LLNode* newCurr = (dLinkedList->current)->next;
        // ADJUST POINTERS
        ((dLinkedList->current)->previous)->next = (dLinkedList->current)->next;
        ((dLinkedList->current)->next)->previous = (dLinkedList->current)->previous;
        // FREE MEMORY
        void *data = (dLinkedList->current)->data;
        free(dLinkedList->current);
        // UPDATE SIZE AND CURR
        dLinkedList->size--;
        dLinkedList->current = newCurr;
        return data;
    }
}


void destroyList(DLinkedList* dLinkedList) {
    if(dLinkedList->head != NULL) {
        getHead(dLinkedList);
        while(deleteForward(dLinkedList)){};
    }
    free(dLinkedList);
}

void* getHead(DLinkedList* dLinkedList) {
    dLinkedList->current = dLinkedList->head;
    return (dLinkedList->head == NULL) ? NULL : (dLinkedList->head)->data;
}

void* getTail(DLinkedList* dLinkedList) {
    dLinkedList->current = dLinkedList->tail;
    return (dLinkedList->tail == NULL) ? NULL : (dLinkedList->tail)->data;
}


void* getCurrent(DLinkedList* dLinkedList) {
    return (dLinkedList->current == NULL) ? NULL : (dLinkedList->current)->data;
}

void* getNext(DLinkedList* dLinkedList) {
    if (dLinkedList->current == NULL) {              // current is null
        return NULL;
    } else if (dLinkedList->current == dLinkedList->tail) {// next is null (curr is tail)
        dLinkedList->current = NULL;
        return NULL;
    } else {                                                 // normal case
        dLinkedList->current = (dLinkedList->current)->next; // update current
        return (dLinkedList->current)->data;                 // return data
    }
}

void* getPrevious(DLinkedList* dLinkedList) {
    if (dLinkedList->current == NULL) {                   // current is null
        return NULL;
    } else if (dLinkedList->current == dLinkedList->head) {// (curr is head)
        dLinkedList->current = NULL;
        return NULL;
    } else {                                        // normal case
        dLinkedList->current = (dLinkedList->current)->previous;
        return (dLinkedList->current)->data;
    }
}

int getSize(DLinkedList* dLinkedList) {
    return dLinkedList->size;
}