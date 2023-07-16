/*
 * Copyright ©2023 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>
#include <stdlib.h>

#include "CSE333.h"
#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {
  // Allocate the linked list record.
  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
  Verify333(ll != NULL);

  // STEP 1: initialize the newly allocated record structure.
  ll->num_elements = 0;   // check if this is the right way to initialize an
                          // instance of a struct
  ll->head = NULL;
  ll->tail = NULL;

  // Return our newly minted linked list.
  return ll;
  }

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {
  Verify333(list != NULL);
  Verify333(payload_free_function != NULL);

  // STEP 2: sweep through the list and free all of the nodes' payloads
  // (using the payload_free_function supplied as an argument) and
  // the nodes themselves.

  while (list->head != NULL) {
    LinkedListNode *curr = list->head;  // Free the node payload
    payload_free_function(curr->payload);
    list->head = list->head->next;  // Free the actual node
    free(curr);
  }
  // Free the LinkedList
  free(list);
}

int LinkedList_NumElements(LinkedList *list) {
  Verify333(list != NULL);
  return list->num_elements;
}

void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;
  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1;
  } else {
    // STEP 3: typical case; list has >=1 elements
    ln->next = list->head;
    ln->prev = NULL;
    list->head->prev = ln;
    list->head = ln;
    list->num_elements += 1;
  }
}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);
  // STEP 4: implement LinkedList_Pop.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().
  if (list->num_elements == 0) {
    return false;
  }
  // Payload_ptr needs to store the payload of the thing that got popped
  *payload_ptr = list->head->payload;

  // Now the payload is stored, so pop the element and return true.
  if (list-> num_elements == 1) {  // the list only has 1 element
    LinkedListNode *temp = list->head;
    list->head = NULL;
    list->tail = NULL;
    free(temp);  // Free payload
  } else {  // The list has greater than or equal to 2 elements:
    LinkedListNode *temp = list->head;
    list->head = list->head->next;
    temp->next = NULL;
    list->head->prev = NULL;
    free(temp);  // Free heap space previously alloc for payload
  }
  list->num_elements -= 1;  // Update number of elements
  return true;  // You may need to change this return value
}


void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // STEP 5: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.

  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;
  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    ln->next = ln->prev = NULL;
    list->head = list->tail = ln;
    list->num_elements = 1;
  } else {
    // Case where the list is not empty
    Verify333(list->head != NULL);
    Verify333(list->tail != NULL);
    list->tail->next = ln;
    ln->prev = list->tail;
    list->tail = list->tail->next;
    list->num_elements += 1;  // Update number of elements
  }
}

void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  Verify333(list != NULL);
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  Verify333(list != NULL);

  // OK, let's manufacture an iterator.
  LLIterator *li = (LLIterator *) malloc(sizeof(LLIterator));
  Verify333(li != NULL);

  // Set up the iterator.
  li->list = list;
  li->node = list->head;

  return li;
}

void LLIterator_Free(LLIterator *iter) {
  Verify333(iter != NULL);
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);

  return (iter->node != NULL);
}

bool LLIterator_Next(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 6: try to advance iterator to the next node and return true if
  // you succeed, false otherwise
  // Note that if the iterator is already at the last node,
  // you should move the iterator past the end of the list
  if (iter->node->next == NULL) {  // itr at last node, set node to NULL
    iter->node = NULL;             // then return false
    return false;
  }

  // Here, we are not at the last node, so move node to the next
  // node and return true
  iter->node = iter->node->next;
  return true;
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  *payload = iter->node->payload;
}

bool LLIterator_Remove(LLIterator *iter,
                       LLPayloadFreeFnPtr payload_free_function) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 7: implement LLIterator_Remove.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.

  if (!LLIterator_IsValid(iter)) {
    return false;
  }
  LinkedListNode *temp = iter->node;
  if (iter->node->prev == NULL && iter->node->next == NULL) {
    // List has only 1 element
    // Free the payload the iterator is point to
    payload_free_function(iter->node->payload);
    iter->node = NULL;  // Set node we're at to null
    iter->list->head = NULL;
    iter->list->tail = NULL;
    free(temp);  // Free the node we're at
    // Update element count
    iter->list->num_elements = 0;
    return false;
  } else if (iter->node->prev == NULL) {  // iter points at head
    // Free the payload the iterator is point to
    payload_free_function(iter->node->payload);
    iter->list->head = iter->node->next;
    iter->list->head->prev = NULL;
    iter->node = iter->list->head;
    iter->list->num_elements -= 1;  // Update number of elements
    free(temp);  // Free the node we're at
    return true;
  } else if (iter->node->next == NULL) {  // iter points at tail
    // Free the payload the iterator is point to
    payload_free_function(iter->node->payload);
    iter->list->tail = iter->node->prev;
    iter->list->tail->next = NULL;
    iter->node = iter->list->tail;
    iter->list->num_elements -= 1;  // Update number of elements
    free(temp);  // Free the node we're at
    return true;
  } else {  // iter points to the middle
    // Free the payload the iterator is point to
    payload_free_function(iter->node->payload);
    iter->node->prev->next = iter->node->next;
    iter->node->next->prev = iter->node->prev;
    iter->node = iter->node->next;
  }
  iter->list->num_elements -= 1;  // Update number of elements
  free(temp);  // Free the node we're at
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Helper functions

bool LinkedList_Slice(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 8: implement LinkedList_Slice.
  if (list->num_elements == 0) {
    return false;
  }

  *payload_ptr = list->tail->payload;

  // Now the payload is stored, so pop the element and return true.
  if (list-> num_elements == 1) {  // the list only has 1 element
    LinkedListNode *temp = list->tail;
    list->head = NULL;  // set head of list to null
    list->tail = NULL;  // set tail of list to null
    free(temp);  // free payload
  } else {  // the list has greater than or equal to 2 elements:
    LinkedListNode *temp = list->tail;
    temp->prev->next = NULL;
    list->tail = list->tail->prev;
    list->tail->prev = NULL;
    free(temp);  // free heap space previously alloc for payload
  }
  list->num_elements -= 1;
  return true;  // you may need to change this return value
}

void LLIterator_Rewind(LLIterator *iter) {
  iter->node = iter->list->head;
}
