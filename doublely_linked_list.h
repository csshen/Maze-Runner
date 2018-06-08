/** @file doublely_linked_list.h */
#ifndef DOUBLELINKEDLIST_H
#define DOUBLELINKEDLIST_H


/********************************************
 * Doublely Linked List library functions *
 * For more details on their functionality, *
 * see the documentation in doublely_linked_list.c *
 ********************************************/


/**
 * This structure represents an entire linked list.
 */
typedef struct dlinkedlist_t {
    /** The head pointer for the list (points to the first node) */
    struct llnode_t* head;

    /** The tail pointer for the list (points to the last node) */
    struct llnode_t* tail;

    /** The current pointer for the list (points to the current node) */
    struct llnode_t* current;

    /** The number of nodes in the list */
    int size;
} DLinkedList;

/**
 * This structure represents a single list node.
 */
typedef struct llnode_t {
    /** The data associated with this node */
    void* data;

    /** A pointer to the previous node in the list. NULL if there is no previous node. */
    struct llnode_t* previous;

    /** A pointer to the next node in the list. NULL if there is no next node. */
    struct llnode_t* next;
} LLNode;


/**
 * create_dlinkedlist
 *
 * Creates a doublely liked list by allocating memory for it on the heap. Initialize the size to zero,
 * as well as head, current, and tail pointer to NULL
 *
 * @return A pointer to an empty dlinkedlist
 */
DLinkedList* create_dlinkedlist(void);

/**
 * create_llnode
 *
 * Helper function that creates a node by allocating memory for it on the heap,
 * and initializing its previous and next pointers to NULL and its data pointer to the input
 * data pointer
 *
 * @param data A void pointer to data the user is adding to the doublely linked list.
 * @return A pointer to the linked list node
 */
LLNode* create_llnode(void* data);


/**
 * InsertHead
 *
 * Insert the data to the head of the doublely linked list.
 * Do not update the current node.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @param data A void pointer to data the user is adding to the doublely linked list.
 *
 */
void insertHead(DLinkedList* dLinkedList, void* data);


/**
 * insertTail
 *
 * Insert the data to the tail of the doublely linked list.
 * Do not update the current node.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @param data A void pointer to data the user is adding to the doublely linked list.
 *
 */
void insertTail(DLinkedList* dLinkedList, void* data);


/**
 * insertAfter
 *
 * Insert a new node into the doublely linked list immediately after the current node.
 * If the current node is NULL, this method fails. Do not update the current node.
 *
 * @param dLinkedList A pointer to the doublely linked list.
 * @param newData A void pointer to the new data to insert.
 * @return 1 if inserted the new data successfully
 *         0 if the current pointer is NULL
 */
int insertAfter(DLinkedList* dLinkedList, void* newData);


/**
 * insertBefore
 *
 * Insert a new node into the doublely linked list immediately before the current node.
 * If the current node is NULL, this method fails. Do not update the current node.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @param newData A void pointer to the new data to insert
 * @return 1 if insert the new data successfully
 *         0 if the current pointer is NULL
 */
int insertBefore(DLinkedList* dLinkedList, void* newData);


/**
 * deleteBackward
 *
 * Delete the node the current pointer is pointed at, and move the current pointer backward.
 * Be aware that deleteBackward will cause problems if the current node is the list head,
 * or if the current node is the only node in the list. In these cases, the current
 * pointer should be set to NULL after deleting the node, and the list head and tail
 * pointers should be updated appropriately.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the data of the new current pointer and NULL if the current pointer is NULL
 */
void* deleteBackward(DLinkedList* dLinkedList);


/**
 * deleteForward
 *
 * Delete the node the current pointer is pointed at, and move the current pointer forward.
 * Be aware that deleteForward will cause problems if the current node is the list tail,
 * or if the current node is the only node in the list. In these cases, the current
 * pointer should be set to NULL after deleting the node, and the list head and tail
 * pointers should be updated appropriately.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the data of the new current pointer and NULL if the current pointer is NULL
 */
void* deleteForward(DLinkedList* dLinkedList);

/**
 * removeBackward
 *
 * Remove the node the current pointer is pointed at from the list and return the data
 * associated with the current node. Move the current pointer backward.
 * Be aware that removeBackward will cause problems if the current node is the list head,
 * or if the current node is the only node in the list. In these cases, the current
 * pointer should be set to NULL after removing the node, and the list head and tail
 * pointers should be updated appropriately.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the data of the removed node, or NULL if the current pointer is NULL
 */
void* removeBackward(DLinkedList* dLinkedList);


/**
 * removeForward
 *
 * Remove the node the current pointer is pointed at from the list and return the data
 * associated with the current node. Move the current pointer forward.
 * Be aware that removeForward will cause problems if the current node is the list tail,
 * or if the current node is the only node in the list. In these cases, the current
 * pointer should be set to NULL after removing the node, and the list head and tail
 * pointers should be updated appropriately.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the data of the removed node, or NULL if the current pointer is NULL
 */
void* removeForward(DLinkedList* dLinkedList);


/**
 * destroyList
 *
 * Destroy the doublely linked list. Everything in the linked list including list structure,
 * nodes and data are all freed from the heap
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @param shouldFree Flag. 1 indicates if data should be freed upon deletion of node.
 *
 */
void destroyList(DLinkedList* dLinkedList);


/**
 * getHead
 *
 * Return the data contained in the head of the doublely linked list, and set the list current pointer
 * to head
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the head data or  NULL if head == NULL
 */
void* getHead(DLinkedList* dLinkedList);

/**
 * getTail
 *
 * Return the data contained in the tail of the doublely linked list, and set the current pointer
 * to tail
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the tail data or  NULL if tail == NULL
 */
void* getTail(DLinkedList* dLinkedList);


/**
 * getCurrent
 *
 * Return the data the current pointer is pointing at
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the current data or NULL if current == NULL
 */
void* getCurrent(DLinkedList* dLinkedList);


/**
 * getNext
 *
 * Return the next data the current pointer is pointing at, and move the current pointer to next node.
 * If the current pointer is at the tail, the next node is NULL.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the next data or NULL if current == NULL
 */
void* getNext(DLinkedList* dLinkedList);


/**
 * getPrevious
 *
 * Return the previous data the current pointer is pointing at, and move the current pointer to
 * previous node.  If the current pointer is at the head, the previous node is NULL.
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return the previous data or NULL if current == NULL
 */
void* getPrevious(DLinkedList* dLinkedList);


/**
 * getSize
 *
 * Return the size of the doublely linked list
 *
 * @param dLinkedList A pointer to the doublely linked list
 * @return  the size
 */
int getSize(DLinkedList* dLinkedList);
#endif
