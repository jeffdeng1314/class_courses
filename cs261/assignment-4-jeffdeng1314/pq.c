/*
 * In this file, you will write the structures and functions needed to
 * implement a priority queue.  Feel free to implement any helper functions
 * you need in this file to implement a priority queue.  Make sure to add your
 * name and @oregonstate.edu email address below:
 *
 * Name:
 * Email:
 */

#include <stdlib.h>
#include <stdio.h>
#include "dynarray.h"
#include <assert.h>

#include "pq.h"

/*
 * This is the structure that represents a priority queue.  You must define
 * this struct to contain the data needed to implement a priority queue.
 */
struct pq{

	struct dynarray* value;
	struct dynarray* priority;

};


/*
 * This function should allocate and initialize an empty priority queue and
 * return a pointer to it.
 */
struct pq* pq_create() {

	struct pq* initialize = malloc(sizeof(struct pq));
	initialize->value = dynarray_create();
	initialize->priority = dynarray_create();
  return initialize;
}


/*
 * This function should free the memory allocated to a given priority queue.
 * Note that this function SHOULD NOT free the individual elements stored in
 * the priority queue.  That is the responsibility of the caller.
 *
 * Params:
 *   pq - the priority queue to be destroyed.  May not be NULL.
 */
void pq_free(struct pq* pq) {
	dynarray_free(pq->value);
	dynarray_free(pq->priority);
	free(pq);

}


/*
 * This function should return 1 if the specified priority queue is empty and
 * 0 otherwise.
 *
 * Params:
 *   pq - the priority queue whose emptiness is to be checked.  May not be
 *     NULL.
 *
 * Return:
 *   Should return 1 if pq is empty and 0 otherwise.
 */
int pq_isempty(struct pq* pq) {
	if(dynarray_size(pq->value) && dynarray_size(pq->priority) == 0)
  return 1;

	else
		return 0;
}



void percolate_up(struct pq* pq, int last,struct pq* temp){
//printf("inside per\n");

	int parent = ((last - 1)/2);
	


	while(dynarray_get(pq->priority,last) < dynarray_get(pq->priority,parent)){
		temp->priority = dynarray_get(pq->priority,parent);
		temp->value = dynarray_get(pq->value,parent);
		

		dynarray_set(pq->priority,parent, dynarray_get(pq->priority,last));
		dynarray_set(pq->value,parent,dynarray_get(pq->value,last));


		// pq->priority[parent] = pq->priority[last];
		// pq->value[parent] = pq->value[last];
		
		dynarray_set(pq->priority,last,temp->priority);
		dynarray_set(pq->value,last,temp->value);
		// pq->priority[last] = temp->priority;
		// pq->value[parent] = temp->value;

	}

	// int* pr = dynarray_get(pq->priority,0);
	// 	int* va = dynarray_get(pq->value,0);

	// printf("va: %d\n", *va);
	// 	printf("pr in percolate: %d\n", *pr);

}


/*
 * This function should insert a given element into a priority queue with a
 * specified priority value.  Note that in this implementation, LOWER priority
 * values are assigned to elements with HIGHER priority.  In other words, the
 * element in the priority queue with the LOWEST priority value should be the
 * FIRST one returned.
 *
 * Params:
 *   pq - the priority queue into which to insert an element.  May not be
 *     NULL.
 *   value - the value to be inserted into pq.
 *   priority - the priority value to be assigned to the newly-inserted
 *     element.  Note that in this implementation, LOWER priority values
 *     should correspond to elements with HIGHER priority.  In other words,
 *     the element in the priority queue with the LOWEST priority value should
 *     be the FIRST one returned.
 */
void pq_insert(struct pq* pq, void* value, int priority) {

	int size = dynarray_size(pq->value);
	// int *p = value;
	// printf("value: %d\n",*p);

struct pq* temp;

	dynarray_insert(pq->value,size,value);
	dynarray_insert(pq->priority,size, &priority);

	percolate_up(pq,size,temp);

	//pq_free(temp);

}


/*
 * This function should return the value of the first item in a priority
 * queue, i.e. the item with LOWEST priority value.
 *
 * Params:
 *   pq - the priority queue from which to fetch a value.  May not be NULL or
 *     empty.
 *
 * Return:
 *   Should return the value of the first item in pq, i.e. the item with
 *   LOWEST priority value.
 */
void* pq_first(struct pq* pq) {

int* x = dynarray_get(pq->value,0);
//printf("x: %d\n",*x );

  return (dynarray_get(pq->value,0));
}


/*
 * This function should return the priority value of the first item in a
 * priority queue, i.e. the item with LOWEST priority value.
 *
 * Params:
 *   pq - the priority queue from which to fetch a priority value.  May not be
 *     NULL or empty.
 *
 * Return:
 *   Should return the priority value of the first item in pq, i.e. the item
 *   with LOWEST priority value.
 */
int pq_first_priority(struct pq* pq) {
	//int i = 0;
	int* prior = dynarray_get(pq->priority,0);

	//printf("prior: %d\n",*prior);

	// 	int* pr = dynarray_get(pq->priority,1);
	// 	int* va = dynarray_get(pq->value,1);

	// printf("va: %d\n", *va);
	// 	printf("pr: %d\n", *pr);
//printf("prior: %d\n",*prior);

  return (*prior);
//	return NULL;
}


void percolate_down(struct pq* pq, int curr){

	int left = (2*(curr)+1);
	int right = (2*(curr)+2);
	int* temp;

	while(left < dynarray_size(pq->value)&& right < dynarray_size(pq->value)){

		if(dynarray_get(pq->priority,left) < dynarray_get(pq->priority,curr)){

				temp = dynarray_get(pq->priority,left);
				dynarray_set(pq->priority,left,dynarray_get(pq->priority,curr));
				dynarray_set(pq->priority,curr,dynarray_get(pq->priority,*temp));

				temp = dynarray_get(pq->value,left);
				dynarray_set(pq->value,left,dynarray_get(pq->value,curr));
				dynarray_set(pq->value,curr,dynarray_get(pq->value,*temp));
		}

		else if(dynarray_get(pq->priority,right) < dynarray_get(pq->priority,curr)){

				temp = dynarray_get(pq->priority,right);
				dynarray_set(pq->priority,right,dynarray_get(pq->priority,curr));
				dynarray_set(pq->priority,curr,dynarray_get(pq->priority,*temp));

				temp = dynarray_get(pq->value,left);
				dynarray_set(pq->value,right,dynarray_get(pq->value,curr));
				dynarray_set(pq->value,curr,dynarray_get(pq->value,*temp));
		}

			break;
	
		}

}


/*
 * This function should return the value of the first item in a priority
 * queue, i.e. the item with LOWEST priority value, and then remove that item
 * from the queue.
 *
 * Params:
 *   pq - the priority queue from which to remove a value.  May not be NULL or
 *     empty.
 *
 * Return:
 *   Should return the value of the first item in pq, i.e. the item with
 *   LOWEST priority value.
 */
void* pq_remove_first(struct pq* pq) {

	int* x = dynarray_get(pq->value,0);
	int size = dynarray_size(pq->value);

	int curr = 0;


	dynarray_set(pq->value,0,(size));

	dynarray_set(pq->priority,0,(size));

	dynarray_remove(pq->value,0);
	dynarray_remove(pq->priority,0);

	percolate_down(pq,curr);



  return x;
}
//                                                      