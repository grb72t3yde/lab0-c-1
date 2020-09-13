#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (!q) {
        return;
    }
    list_ele_t *index = q->head;
    while (q->head) {
        q->head = q->head->next;
        free(index->value);
        free(index);
        index = q->head;
    }
    free(q);
    index = NULL;
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }
    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }
    newh->value = malloc(strlen(s) * sizeof(char) +
                         1); /* Allocate addtional byte for '\0' */
    if (!newh->value) {
        free(newh);
        newh = NULL;
        return false;
    }
    memset(newh->value, 0, strlen(s) + 1);
    strncpy(newh->value, s, strlen(s));
    newh->next = q->head;
    q->head = newh;
    if (!q->tail) {
        q->tail = newh;
    }
    q->size++;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q) {
        return false;
    }
    list_ele_t *newh;
    newh = malloc(sizeof(list_ele_t));
    if (!newh) {
        return false;
    }
    newh->value = malloc(strlen(s) * sizeof(char) + 1);
    if (!newh->value) {
        free(newh);
        newh = NULL;
        return false;
    }
    memset(newh->value, 0, strlen(s) + 1);
    strncpy(newh->value, s, strlen(s));
    newh->next = NULL;
    if (!q->head) {
        q->head = q->tail = newh;
    } else {
        q->tail->next = newh;
        q->tail = q->tail->next;
    }
    q->size++;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head) {
        return false;
    }
    list_ele_t *index = q->head;
    if (sp) {
        size_t maxsize = (strlen(index->value) > bufsize - 1)
                             ? bufsize - 1
                             : strlen(index->value);
        memset(sp, 0,
               maxsize + 1); /* Additional byte copy for the terminator */
        strncpy(sp, index->value, maxsize);
    }
    q->head = q->head->next;
    q->size--;
    free(index->value);
    free(index);
    index = NULL;
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head) {
        return;
    }
    if (q->size == 1) {
        return;
    }
    list_ele_t *prev, *cur, *foward;
    prev = q->head;
    cur = prev->next;
    foward = cur->next;

    while (foward) {
        cur->next = prev;
        prev = cur;
        cur = foward;
        foward = foward->next;
    }
    cur->next = prev;
    q->tail = q->head;
    q->head->next = NULL;
    q->head = cur;
}

void split_and_merge(list_ele_t **head)
{
    if (!(*head)->next) { /* When the list has one node left, bound the
                             recurssion */
        return;
    }
    list_ele_t *lhead = *head;
    list_ele_t *rhead = (*head)->next;
    /* Make rhead point to the mid of the list */
    while (rhead && rhead->next) { /* Thid condition avoid illegal access */
        rhead = rhead->next->next;
        lhead = lhead->next;
    }
    rhead = lhead->next;
    lhead->next = NULL;
    lhead = *head;
    split_and_merge(&lhead);
    split_and_merge(&rhead);

    *head = NULL;
    list_ele_t **appender = head; /* This appender is used to manipulate
                                     the pointers in the original list */
    /* Mergint two lists using local reference */
    while (lhead && rhead) {
        if (strcmp(lhead->value, rhead->value) < 0) {
            (*appender) = lhead;
            lhead = lhead->next;
            (*appender)->next = NULL;
        } else {
            (*appender) = rhead;
            rhead = rhead->next;
            (*appender)->next = NULL;
        }
        appender = &((*appender)->next); /* This is a pointer to the "pointer of
                                     next" in the list node structure */

    }
    /* Append the remained list */
    *appender = lhead ? lhead : rhead;
}
/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
void q_sort(queue_t *q)
{
    if (!q || q->size <= 1) {
        return;
    }
    split_and_merge(&q->head);
    while (q->tail->next) {
        q->tail = q->tail->next;
    }
}
