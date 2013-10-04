/* 
 * Copyright 2011 Steven Gribble
 *
 *  This file is part of the UW CSE 333 course project sequence
 *  (333proj).
 *
 *  333proj is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  333proj is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with 333proj.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Assert333.h"
#include "HashTable.h"
#include "HashTable_priv.h"

// A private utility function to grow the hashtable (increase
// the number of buckets) if its load factor has become too high.
static void ResizeHashtable(HashTable ht);

// a free function that does nothing
static void NullFree(void *freeme) { }

// a private utility function used to find the key/value pair associated with a
// given key.
//
// Arguments:
//
// -table:  the table to query
//
// -key:  the key associated with a key/value pair that may be in 
//  table being queried
// 
// -KVptr: return parameter through which the address of the found
//  key/value pair is stored.
//  
//  Returns:
//
// - 0 if the key/value pair is not found. In this case the return
//  parameter is set to NULL.
//
// - +1 if the key/value pair is found. In this case the return 
//  parameter is set to the address of the found key/value pair
//
//  - -1 if there is a failure (e.g. out of memory). In this case
//  the return parameter is set to NULL.
static int FindKey(HashTable table, uint64_t key, HTKeyValue **KVptr);

HashTable AllocateHashTable(uint32_t num_buckets) {
	HashTable ht;
	uint32_t  i;

	// defensive programming
	if (num_buckets == 0) {
		return NULL;
	}

	// allocate the hash table record
	ht = (HashTable) malloc(sizeof(HashTableRecord));
	if (ht == NULL) {
		return NULL;
	}

	// initialize the record
	ht->num_buckets = num_buckets;
	ht->num_elements = 0;
	ht->buckets =
		(LinkedList *) malloc(num_buckets * sizeof(LinkedList));
	if (ht->buckets == NULL) {
		// make sure we don't leak!
		free(ht);
		return NULL;
	}
	for (i = 0; i < num_buckets; i++) {
		ht->buckets[i] = AllocateLinkedList();
		if (ht->buckets[i] == NULL) {
			// allocating one of our bucket chain lists failed,
			// so we need to free everything we allocated so far
			// before returning NULL to indicate failure.  Since
			// we know the chains are empty, we'll pass in a
			// free function pointer that does nothing; it should
			// never be called.
			uint32_t j;
			for (j = 0; j < i; j++) {
				FreeLinkedList(ht->buckets[j], NullFree);
			}
			free(ht);
			return NULL;
		}
	}

	return (HashTable) ht;
}

void FreeHashTable(HashTable table,
		ValueFreeFnPtr value_free_function) {
	uint32_t i;

	Assert333(table != NULL);  // be defensive

	// loop through and free the chains on each bucket
	for (i = 0; i < table->num_buckets; i++) {
		LinkedList  bl = table->buckets[i];
		HTKeyValue *nextKV;

		// pop elements off the the chain list, then free the list
		while (NumElementsInLinkedList(bl) > 0) {
			Assert333(PopLinkedList(bl, (void **) &nextKV));
			value_free_function(nextKV->value);
			free(nextKV);
		}
		// the chain list is empty, so we can pass in the
		// null free function to FreeLinkedList.
		FreeLinkedList(bl, NullFree);
	}

	// free the bucket array within the table record,
	// then free the table record itself.
	free(table->buckets);
	free(table);
}

uint64_t NumElementsInHashTable(HashTable table) {
	Assert333(table != NULL);
	return table->num_elements;
}

uint64_t FNVHash64(unsigned char *buffer, unsigned int len) {
	// This code is adapted from code by Landon Curt Noll
	// and Bonelli Nicola:
	//
	// http://code.google.com/p/nicola-bonelli-repo/
	static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
	static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
	unsigned char *bp = (unsigned char *) buffer;
	unsigned char *be = bp + len;
	uint64_t hval = FNV1_64_INIT;

	/*
	 * FNV-1a hash each octet of the buffer
	 */
	while (bp < be) {
		/* xor the bottom with the current octet */
		hval ^= (uint64_t) * bp++;
		/* multiply by the 64 bit FNV magic prime mod 2^64 */
		hval *= FNV_64_PRIME;
	}
	/* return our new hash value */
	return hval;
}

uint64_t FNVHashInt64(uint64_t hashme) {
	unsigned char buf[8];
	int i;

	for (i = 0; i < 8; i++) {
		buf[i] = (unsigned char) (hashme & 0x00000000000000FFULL);
		hashme >>= 8;
	}
	return FNVHash64(buf, 8);
}

uint64_t HashKeyToBucketNum(HashTable ht, uint64_t key) {
	return key % ht->num_buckets;
}

int InsertHashTable(HashTable table,
		HTKeyValue newkeyvalue,
		HTKeyValue *oldkeyvalue) {
	Assert333(table != NULL);
	ResizeHashtable(table);
	// calculate which bucket we are inserting into,
	uint64_t insertbucket = HashKeyToBucketNum(table, newkeyvalue.key);
	// grab its linked list chain
	LinkedList insertchain = table->buckets[insertbucket];
	// simple case: there are no elements in the bucket yet, so push value onto linked list
	if (NumElementsInLinkedList(insertchain) == 0) {
		HTKeyValue *newKV = (HTKeyValue *) malloc(sizeof(HTKeyValue));
		if (newKV == NULL) {
		// malloc failure
			return 0;
		}
		*newKV = newkeyvalue;
		PushLinkedList(insertchain, newKV);
		table->num_elements++;
		return 1;
		// It is more difficult if the linked list contains items because 
		// an item with the same key might be present and have its value replace
		// and the old value returned through the return parameter
	} else {
		HTKeyValue *KVptr;
		// attempt to find they key/value pair
		int status = FindKey(table, newkeyvalue.key, &KVptr);
		if (status == 0) {
		// if not found, add newkeyvalue to the hashtable
			HTKeyValue *newKV = (HTKeyValue *) malloc(sizeof(HTKeyValue));
			*newKV = newkeyvalue;
			PushLinkedList(insertchain, newKV);
			table->num_elements++;
			return 1;
		}else if (status == 1){
			*oldkeyvalue = *KVptr;
			KVptr->value = newkeyvalue.value;
			return 2;
		}else {
			return 0;
		}
	}
}

static int FindKey(HashTable table, uint64_t key, HTKeyValue **KVptr) {
	//determine which bucket the key would be in
	uint64_t bucket = HashKeyToBucketNum(table, key);
	//get the LinkedList chain in which the key may be stored
	LinkedList chain = table->buckets[bucket];
	// If the chain for this bucket is empty then the key/value pair is not in the table
	if (NumElementsInLinkedList(chain) == 0) {
		*KVptr = NULL;
		return 0;	
	}else {	
		// Iterate over the chain in search of the key/value pair
		LLIter iter = LLMakeIterator(chain, 0);
		if (iter == NULL) {
			// iterator creation failure
			*KVptr = NULL;
			return -1;
		}do {
			HTKeyValue *curKV;
			LLIteratorGetPayload(iter,(void**) &curKV);
			if (curKV->key == key) {
				LLIteratorFree(iter);
				*KVptr = curKV;
				return 1;
			}	
		} while (LLIteratorNext(iter));
		LLIteratorFree(iter);
	}
	*KVptr = NULL;
	return 0;
}

int LookupHashTable(HashTable table,
		uint64_t key,
		HTKeyValue *keyvalue) {
	Assert333(table != NULL);
	//uint64_t bucket = HashKeyToBucketNum(table, key);
	HTKeyValue *KVptr;
	int status = FindKey(table, key, &KVptr);
	if (status == 0) {
		// key/value pair not found
		return 0;
	} else if (status == 1){
		// key found
		*keyvalue = *KVptr;
		return 1;	
	} else {
		// failure
		return -1;
	}	
}
void HTKVfreeFn(HTKeyValue *KVtoBeFreed) {
	free(KVtoBeFreed);
}
int RemoveFromHashTable(HashTable table,
		uint64_t key,
		HTKeyValue *keyvalue) {
	Assert333(table != NULL);
	//determine which bucket the key would be in
	uint64_t bucket = HashKeyToBucketNum(table, key);
	//get the LinkedList chain in which the key may be stored
	LinkedList chain = table->buckets[bucket];
	if (NumElementsInLinkedList(chain) == 0) {
		return 0;
	}
	LLIter iter = LLMakeIterator(chain, 0);
	if (iter == NULL) {
		// Iterator creation failed
		return -1;
	}
	do {
		HTKeyValue *curKV;
		LLIteratorGetPayload(iter, (void**) &curKV);
		if (curKV->key == key) {
			*keyvalue = *curKV;
			LLIteratorDelete(iter, (LLPayloadFreeFnPtr) &HTKVfreeFn);
			table->num_elements--;
			LLIteratorFree(iter);
			return 1;
		}	
	} while (LLIteratorNext(iter));
	LLIteratorFree(iter);
	return 0;
}

HTIter HashTableMakeIterator(HashTable table) {
	HTIterRecord *iter;
	uint32_t      i;

	Assert333(table != NULL);  // be defensive

	// malloc the iterator
	iter = (HTIterRecord *) malloc(sizeof(HTIterRecord));
	if (iter == NULL) {
		return NULL;
	}

	// if the hash table is empty, the iterator is immediately invalid,
	// since it can't point to anything.
	if (table->num_elements == 0) {
		iter->is_valid = false;
		iter->ht = table;
		iter->bucket_it = NULL;
		return iter;
	}

	// initialize the iterator. There is at least one element in the
	// table, so find the first element and point the iterator at it.
	iter->is_valid = true;
	iter->ht = table;
	for (i = 0; i < table->num_buckets; i++) {
		if (NumElementsInLinkedList(table->buckets[i]) > 0) {
			iter->bucket_num = i;
			break;
		}
	}
	Assert333(i < table->num_buckets);  // make sure we found it.
	iter->bucket_it = LLMakeIterator(table->buckets[iter->bucket_num], 0UL);
	if (iter->bucket_it == NULL) {
		// out of memory!
		free(iter);
		return NULL;
	}
	return iter;
}

void HTIteratorFree(HTIter iter) {
	Assert333(iter != NULL);
	if (iter->bucket_it != NULL) {
		LLIteratorFree(iter->bucket_it);
		iter->bucket_it = NULL;
	}
	iter->is_valid = false;
	free(iter);
}

int HTIteratorNext(HTIter iter) {
	Assert333(iter != NULL);
	if (HTIteratorPastEnd(iter) == 1) {
		return 0;
	}
	LLIter bucketIter = iter->bucket_it;
	if (LLIteratorHasNext(bucketIter)) {
		// advance the linked list iterator to the next node
		LLIteratorNext(bucketIter);
		return 1;
	} else {
		// must move to another bucket, or return 0 if there are no more
		uint64_t totalBuckets = iter->ht->num_buckets;
		HashTable table = iter->ht;
		LinkedList *buckets = table->buckets;
		LLIter prevLLIter = iter->bucket_it;
		iter->bucket_num++;
		while (((iter->bucket_num < totalBuckets) && NumElementsInLinkedList(buckets[iter->bucket_num]) == 0)){
			iter->bucket_num++;
		}
		if (iter->bucket_num == totalBuckets) {
			iter->is_valid = false;
			return 0;
		}
		iter->bucket_it = LLMakeIterator(table->buckets[iter->bucket_num], 0);
		LLIteratorFree(prevLLIter);	
		return 1;
	}
}

int HTIteratorPastEnd(HTIter iter) {
	Assert333(iter != NULL);
	if (!(iter->is_valid)) {
		return 1;
	}
	return 0;
}

int HTIteratorGet(HTIter iter, HTKeyValue *keyvalue) {
	Assert333(iter != NULL);
	if (HTIteratorPastEnd(iter) == 1) {
		return 0;
	}
	HTKeyValue * temp;
	LLIteratorGetPayload(iter->bucket_it, (void**) &temp);
	*keyvalue = *temp;	
	return 1;	
}

int HTIteratorDelete(HTIter iter, HTKeyValue *keyvalue) {
	HTKeyValue kv;
	int res, retval;
	Assert333(iter != NULL);
	// Try to get what the iterator is pointing to.
	res = HTIteratorGet(iter, &kv);
	if (res == 0)
		return 0;
	// Advance the iterator.
	res = HTIteratorNext(iter);
	if (res == 0) {
		retval = 2;
	} else {
		retval = 1;
	}
	res = RemoveFromHashTable(iter->ht, kv.key, keyvalue);
	Assert333(res == 1);
	Assert333(kv.key == keyvalue->key);
	Assert333(kv.value == keyvalue->value);

	return retval;
}

static void ResizeHashtable(HashTable ht) {
	// Resize if the load factor is > 3.
	if (ht->num_elements < 3 * ht->num_buckets)
		return;

	// This is the resize case.  Allocate a new hashtable,
	// iterate over the old hashtable, do the surgery on
	// the old hashtable record and free up the new hashtable
	// record.
	HashTable newht = AllocateHashTable(ht->num_buckets * 9);

	// Give up if out of memory.
	if (newht == NULL)
		return;

	// Loop through the old ht with an iterator,
	// inserting into the new HT.
	HTIter it = HashTableMakeIterator(ht);
	if (it == NULL) {
		// Give up if out of memory.
		FreeHashTable(newht, &NullFree);
		return;
	}

	while (!HTIteratorPastEnd(it)) {
		HTKeyValue item, dummy;

		Assert333(HTIteratorGet(it, &item) == 1);
		if (InsertHashTable(newht, item, &dummy) != 1) {
			// failure, free up everything, return.
			HTIteratorFree(it);
			FreeHashTable(newht, &NullFree);
			return;
		}
		HTIteratorNext(it);
	}

	// Worked!  Free the iterator.
	HTIteratorFree(it);

	// Sneaky: swap the structures, then free the new table,
	// and we're done.
	{
		HashTableRecord tmp;

		tmp = *ht;
		*ht = *newht;
		*newht = tmp;
		FreeHashTable(newht, &NullFree);
	}

	return;
}
