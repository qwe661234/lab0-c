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
    if (q == NULL) {
        return NULL;
    }
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    if (q == NULL) {
        return;
    }
    list_ele_t *node;
    node = q->head;
    while (node) {
        list_ele_t *current = node;
        node = node->next;
        free(current->value);
        free(current);
    }
    free(q);
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
    list_ele_t *newh;
    if (q == NULL) {
        return false;
    }
    newh = malloc(sizeof(list_ele_t));
    if (newh == NULL) {
        return false;
    }
    int slen = strlen(s);
    newh->value = malloc((slen + 1) * sizeof(char));
    if (newh->value == NULL) {
        free(newh);
        return false;
    }
    strncpy(newh->value, s, slen);
    newh->value[slen] = '\0';
    newh->next = q->head;
    q->head = newh;
    q->size++;
    if (q->size == 1) {
        q->tail = q->head;
    }
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
    list_ele_t *newt;
    if (q == NULL) {
        return false;
    }
    newt = malloc(sizeof(list_ele_t));
    if (newt == NULL) {
        return false;
    }
    int slen = strlen(s);
    newt->value = malloc((slen + 1) * sizeof(char));
    if (newt->value == NULL) {
        free(newt->value);
        free(newt);
        return false;
    }
    strncpy(newt->value, s, slen);
    newt->value[slen] = '\0';
    newt->next = NULL;
    if (q->size == 0) {
        q->head = newt;
        q->tail = newt;
    } else {
        q->tail->next = newt;
        q->tail = newt;
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
    if (q == NULL || q->head == NULL) {
        return false;
    }
    list_ele_t *target = q->head;
    if (sp != NULL) {
        int slen = strlen(target->value);
        if (bufsize >= (slen + 1)) {
            strncpy(sp, target->value, slen);
            sp[slen] = '\0';
        } else {
            strncpy(sp, target->value, bufsize - 1);
            sp[bufsize - 1] = '\0';
        }
    }
    q->head = q->head->next;
    q->size--;
    if (q->size == 0) {
        q->tail = NULL;
    }
    free(target->value);
    free(target);
    return true;
}

/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    if (q == NULL) {
        return 0;
    }
    return q->size;
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
    if (q == NULL || q->head == q->tail) {
        return;
    }
    list_ele_t *front, *back, *temp;
    front = q->head;
    back = front->next;
    q->tail = q->head;
    while (back != NULL) {
        temp = back->next;
        back->next = front;
        if (front == q->head) {
            front->next = NULL;
        }
        front = back;
        back = temp;
    }
    q->head = front;
}

/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    list_ele_t *newh = l1;
    if (strcmp(l1->value, l2->value) <= 0) {
        l1 = l1->next;
    } else {
        newh = l2;
        l2 = l2->next;
    }
    list_ele_t *ptr = newh;
    while (1) {
        if (l1 != NULL && l2 != NULL) {
            if (strcmp(l1->value, l2->value) <= 0) {
                ptr->next = l1;
                l1 = l1->next;
            } else {
                ptr->next = l2;
                l2 = l2->next;
            }
        } else {
            if (l2 == NULL) {
                ptr->next = l1;
            } else {
                ptr->next = l2;
            }
            break;
        }
        ptr = ptr->next;
    }
    return newh;
}
list_ele_t *mergeSortList(list_ele_t *head)
{
    // merge sort
    if (head == NULL || head->next == NULL) {
        return head;
    }
    list_ele_t *fast = head->next;
    list_ele_t *slow = head;
    // split list
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    list_ele_t *l1 = mergeSortList(head);
    list_ele_t *l2 = mergeSortList(fast);

    // merge sorted l1 and sorted l2
    return merge(l1, l2);
}
void q_sort(queue_t *q)
{
    if (q == NULL || q->head == q->tail) {
        return;
    }
    q->head = mergeSortList(q->head);
    list_ele_t *ptr = q->head;
    while (ptr->next != NULL) {
        ptr = ptr->next;
    }
    q->tail = ptr;
}
