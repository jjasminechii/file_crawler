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
#include <stdint.h>

#include "CSE333.h"
#include "HashTable.h"
#include "HashTable_priv.h"

#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// Internal helper functions.
//
#define INVALID_IDX -1

// Iterator through hashtable and returns true if there
// is a matching key found and false otherwise.
static bool HTLL_HasKey(HTKey_t key, HTKeyValue_t *kv, LLIterator *itr);

// Frees the pointer specified by tofree
static void FreePointer(LLPayload_t tofree);

// Grows the hashtable (ie, increase the number of buckets) if its load
// factor has become too high.
static void MaybeResize(HashTable *ht);

int HashKeyToBucketNum(HashTable *ht, HTKey_t key) {
  return key % ht->num_buckets;
}

// Deallocation functions that do nothing.  Useful if we want to deallocate
// the structure (eg, the linked list) without deallocating its elements or
// if we know that the structure is empty.
static void LLNoOpFree(LLPayload_t freeme) { }
static void HTNoOpFree(HTValue_t freeme) { }


///////////////////////////////////////////////////////////////////////////////
// HashTable implementation.

HTKey_t FNVHash64(unsigned char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (uint64_t) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

HashTable* HashTable_Allocate(int num_buckets) {
  HashTable *ht;
  int i;

  Verify333(num_buckets > 0);

  // Allocate the hash table record.
  ht = (HashTable *) malloc(sizeof(HashTable));
  Verify333(ht != NULL);

  // Initialize the record.
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets = (LinkedList **) malloc(num_buckets * sizeof(LinkedList *));
  Verify333(ht->buckets != NULL);
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = LinkedList_Allocate();
  }

  return ht;
}

void HashTable_Free(HashTable *table,
                    ValueFreeFnPtr value_free_function) {
  int i;

  Verify333(table != NULL);

  // Free each bucket's chain.
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList *bucket = table->buckets[i];
    HTKeyValue_t *kv;

    // Pop elements off the chain list one at a time.  We can't do a single
    // call to LinkedList_Free since we need to use the passed-in
    // value_free_function -- which takes a HTValue_t, not an LLPayload_t -- to
    // free the caller's memory.
    while (LinkedList_NumElements(bucket) > 0) {
      Verify333(LinkedList_Pop(bucket, (LLPayload_t *)&kv));
      value_free_function(kv->value);
      free(kv);
    }
    // The chain is empty, so we can pass in the
    // null free function to LinkedList_Free.
    LinkedList_Free(bucket, LLNoOpFree);
  }

  // Free the bucket array within the table, then free the table record itself.
  free(table->buckets);
  free(table);
}

int HashTable_NumElements(HashTable *table) {
  Verify333(table != NULL);
  return table->num_elements;
}

bool HashTable_Insert(HashTable *table,
                      HTKeyValue_t newkeyvalue,
                      HTKeyValue_t *oldkeyvalue) {
  int bucket;
  LinkedList *chain;

  Verify333(table != NULL);
  MaybeResize(table);

  // Calculate which bucket and chain we're inserting into.
  bucket = HashKeyToBucketNum(table, newkeyvalue.key);
  chain = table->buckets[bucket];

  // STEP 1: finish the implementation of InsertHashTable.
  // This is a fairly complex task, so you might decide you want
  // to define/implement a helper function that helps you find
  // and optionally remove a key within a chain, rather than putting
  // all that logic inside here.  You might also find that your helper
  // can be reused in steps 2 and 3.

  // allocate heap space for the newkeyvalue
  HTKeyValue_t *newKV = (HTKeyValue_t*) malloc(sizeof(HTKeyValue_t));
  if (newKV == NULL) {
    return false;
  }
  newKV->key = newkeyvalue.key;
  newKV->value = newkeyvalue.value;
  // Cast newKV to type LLPayload_t
  LLPayload_t* PayloadKV = (LLPayload_t*)(newKV);
  // If the num on elements is 0, we dont need to search, we can add
  if (LinkedList_NumElements(chain) == 0) {
    LinkedList_Append(chain, PayloadKV);
    // If this return false, you need to do something and also free newKY
    table->num_elements += 1;
    return false;
  }
  // Here, the key could be in the chain. make an iterator to traverse
  LLIterator *chainItr = LLIterator_Allocate(chain);
  if (HTLL_HasKey(newkeyvalue.key, oldkeyvalue, chainItr)) {
    LinkedList_Append(chain, PayloadKV);
    LLIterator_Remove(chainItr, FreePointer);
    LLIterator_Free(chainItr);
    return true;
  }
  LLIterator_Free(chainItr);
  LinkedList_Append(chain, PayloadKV);
  table->num_elements += 1;
  return false;
}

static void FreePointer(LLPayload_t tofree) {
  free(tofree);
}

bool HTLL_HasKey(HTKey_t key, HTKeyValue_t *kv, LLIterator *itr) {
  HTKeyValue_t *payload;
  while (LLIterator_IsValid(itr)) {
    LLIterator_Get(itr, (LLPayload_t*) &payload);
    if (payload->key == key) {
      *kv = *payload;
      return true;
    }
    LLIterator_Next(itr);
  }
  return false;
}

bool HashTable_Find(HashTable *table,
                    HTKey_t key,
                    HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);
  // STEP 2: implement HashTable_Find.
  // Find buckets and chain
  int current_bucket = HashKeyToBucketNum(table, key);
  LinkedList *chain = table->buckets[current_bucket];
  // If the hashtable is empty, we return false
  if (table->num_elements == 0) {
    return false;
  }
  // If the bucket is empty, we return false
  if (LinkedList_NumElements(chain) == 0) {
    return false;
  }
  // Find iterator for our chain
  LLIterator *iterator = LLIterator_Allocate(chain);
  // If the iterator for chain is null, we return false
  if (iterator == NULL) {
    return false;
  }
  // If we have a matching key, then we free
  // the iterator for chain and return true.
  if (HTLL_HasKey(key, keyvalue, iterator)) {
    LLIterator_Free(iterator);
    return true;
  }
  // Otherwise free iterator and return false
  LLIterator_Free(iterator);
  return false;
}

bool HashTable_Remove(HashTable *table,
                      HTKey_t key,
                      HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 3: implement HashTable_Remove.
  // check if table is empty
  if (HashTable_NumElements(table) == 0) {
    return false;
  }
  // Find buckets and chain
  int bucket = HashKeyToBucketNum(table, key);
  LinkedList *current_chain = table->buckets[bucket];
  if (LinkedList_NumElements(current_chain) == 0) {
    return false;
  }
  // Find iterator for our current chain
  LLIterator *chained_iterator = LLIterator_Allocate(current_chain);
  HTKeyValue_t *current_key_val;
  // If we have a matching key
  if (HTLL_HasKey(key, keyvalue, chained_iterator)) {
    LLIterator_Get(chained_iterator, (LLPayload_t*) &current_key_val);
    keyvalue->key = current_key_val->key;
    keyvalue->value = current_key_val->value;
    LLIterator_Remove(chained_iterator, LLNoOpFree);
    // Update number of table elements
    table->num_elements = table->num_elements - 1;
    // Free our iterator and current_key_val
    LLIterator_Free(chained_iterator);
    free(current_key_val);
    return true;
  }
  // Free our iterator
  LLIterator_Free(chained_iterator);
  return false;
}


///////////////////////////////////////////////////////////////////////////////
// HTIterator implementation.

HTIterator* HTIterator_Allocate(HashTable *table) {
  HTIterator *iter;
  int         i;

  Verify333(table != NULL);

  iter = (HTIterator *) malloc(sizeof(HTIterator));
  Verify333(iter != NULL);

  // If the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->ht = table;
    iter->bucket_it = NULL;
    iter->bucket_idx = INVALID_IDX;
    return iter;
  }

  // Initialize the iterator.  There is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (LinkedList_NumElements(table->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }
  }
  Verify333(i < table->num_buckets);  // make sure we found it.
  iter->bucket_it = LLIterator_Allocate(table->buckets[iter->bucket_idx]);
  return iter;
}

void HTIterator_Free(HTIterator *iter) {
  Verify333(iter != NULL);
  if (iter->bucket_it != NULL) {
    LLIterator_Free(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  free(iter);
}

bool HTIterator_IsValid(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 4: implement HTIterator_IsValid.
  int current_bucket = iter->bucket_idx;
  int ht_bucket = iter->ht->num_buckets;
  // If our iter is at the last bucket,
  // return false.
  if (current_bucket > ht_bucket - 1) {
    return false;
  }
  // If our table is empty, return false.
  if (HashTable_NumElements(iter->ht) == 0) {
    return false;
  }
  LLIterator *bucket_iter = iter->bucket_it;
  // If our iterator for the bucket is null,
  // return false.
  if (bucket_iter == NULL) {
    return false;
  }
  // If the iterator is past the end of the
  // list, return false
  if (!LLIterator_IsValid(bucket_iter)) {
    return false;
  }
  return true;
}

bool HTIterator_Next(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 5: implement HTIterator_Next.
  // If the iterator is past the end of the
  // table, return false
  if (!HTIterator_IsValid(iter)) {
    return false;
  }
  // Advance our iterator for bucket
  if (!LLIterator_Next(iter->bucket_it)) {
    // Check next buckets
    for (int i = iter->bucket_idx + 1; i < iter->ht->num_buckets; i++) {
      if (LinkedList_NumElements(iter->ht->buckets[i]) > 0) {
        LLIterator_Free(iter->bucket_it);
        iter -> bucket_idx = i;
        iter -> bucket_it = LLIterator_Allocate(iter->ht->buckets[i]);
        return true;
      }
    }
    return false;
  }
  return true;
}

bool HTIterator_Get(HTIterator *iter, HTKeyValue_t *keyvalue) {
  Verify333(iter != NULL);
  // STEP 6: implement HTIterator_Get.
  Verify333(keyvalue != NULL);
  // If the iterator is past the end of the
  // table, return false
  if (!HTIterator_IsValid(iter)) {
    return false;
  }
  HTKeyValue_t *current_key;
  LLPayload_t* get_keyed_current = (LLPayload_t*) &current_key;
  LLPayload_t get_key = get_keyed_current;
  LLIterator_Get(iter->bucket_it, get_key);
  keyvalue->key = current_key->key;
  keyvalue->value = current_key->value;
  return true;
}

bool HTIterator_Remove(HTIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t kv;

  Verify333(iter != NULL);

  // Try to get what the iterator is pointing to.
  if (!HTIterator_Get(iter, &kv)) {
    return false;
  }

  // Advance the iterator.  Thanks to the above call to
  // HTIterator_Get, we know that this iterator is valid (though it
  // may not be valid after this call to HTIterator_Next).
  HTIterator_Next(iter);

  // Lastly, remove the element.  Again, we know this call will succeed
  // due to the successful HTIterator_Get above.
  Verify333(HashTable_Remove(iter->ht, kv.key, keyvalue));
  Verify333(kv.key == keyvalue->key);
  Verify333(kv.value == keyvalue->value);

  return true;
}

static void MaybeResize(HashTable *ht) {
  HashTable *newht;
  HashTable tmp;
  HTIterator *it;

  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  newht = HashTable_Allocate(ht->num_buckets * 9);

  // Loop through the old ht copying its elements over into the new one.
  for (it = HTIterator_Allocate(ht);
       HTIterator_IsValid(it);
       HTIterator_Next(it)) {
    HTKeyValue_t item, unused;

    Verify333(HTIterator_Get(it, &item));
    HashTable_Insert(newht, item, &unused);
  }

  // Swap the new table onto the old, then free the old table (tricky!).  We
  // use the "no-op free" because we don't actually want to free the elements;
  // they're owned by the new table.
  tmp = *ht;
  *ht = *newht;
  *newht = tmp;

  // Done!  Clean up our iterator and temporary table.
  HTIterator_Free(it);
  HashTable_Free(newht, &HTNoOpFree);
}
