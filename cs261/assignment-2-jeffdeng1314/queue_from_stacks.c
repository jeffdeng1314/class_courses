/*
 * This is the file in which you'll write the functions required to implement
 * a queue using two stacks.  Make sure to add your name and @oregonstate.edu
 * email address below:
 *
 * Name: Hao Deng
 * Email:dengh@oregonstate.edu
 */

#include <stdio.h>
#include <assert.h>

#include <stdlib.h>

#include "stack.h"
#include "queue_from_stacks.h"

/*
 * This function should allocate and initialize all of the memory needed for
 * your queue and return a pointer to the queue structure.
 */
struct queue_from_stacks* queue_from_stacks_create() {

	struct queue_from_stacks* queue_stack = malloc(sizeof(struct queue_from_stacks));

	queue_stack->s1 =stack_create();
	queue_stack->s2 =stack_create();



  return queue_stack;
}

/*
 * This function should free all of the memory allocated to a queue, including
 * the memory associated with each stack.
 *
 * Params:
 *   queue - the queue to be destroyed.  May not be NULL.  The function should
 *     exit the program with an error if queue is NULL.
 */
void queue_from_stacks_free(struct queue_from_stacks* queue) {

	assert(queue);

	free(queue->s1);
	free(queue->s2);
	free(queue);

}

/*
 * This function should return 0 if a queue is completely empty or 1 if there
 * is at least one element stored in the queue.
 *
 * Params:
 *   queue - the queue whose emptiness is to be checked.  May not be NULL.
 *     The function should exit the program with an error if queue is NULL.
 *
 * Return:
 *   Should return 0 if the queue is empty or 1 otherwise.
 */
int queue_from_stacks_isempty(struct queue_from_stacks* queue) {
	//if(stack_isempty(queue->s1)&&stack_isempty(queue->s2))
	assert(queue);
  	if(stack_isempty(queue->s1)&&stack_isempty(queue->s2))
  		return 1;
else 
	return 0;
}

/*
 * Should enqueue a new value onto the end of a queue.
 *
 * Params:
 *   queue - the queue onto which to enqueue a value.  May not be NULL.
 *     The function should exit the program with an error if queue is NULL.
 *   value - the new value to be enqueueed onto the queue
 */
void queue_from_stacks_enqueue(struct queue_from_stacks* queue, int value) {

//if(stack_isempty(queue->s2)!= 0)
		assert(queue);

int y;
	//printf("i'm in queue enqueue\n");
		

		while(!stack_isempty(queue->s2)){
				y = stack_pop(queue->s2);
				stack_push(queue->s1,y);
		}

		stack_push(queue->s1,value);
	//printf("after stack push in enqueue\n");
	//if(stack_isempty(queue->s2))
//
	// else
		//stack_push(queue->s2,value);
}

/*
 * Should return a queue's front value without removing that value from the
 * queue.
 *
 * Params:
 *   queue - the queue from which to read the front value.  May not be NULL or
 *     empty.  The function should exit the program with an error if queue is
 *     NULL or empty.
 *
 * Return:
 *   Should return the value stored at the front of the queue.
 */
int queue_from_stacks_front(struct queue_from_stacks* queue) {
		assert(queue);


	int x = 0,y;
	//	printf("x: %d\n",x);

	//z = stack_top(queue->s1);
	//while((queue_from_stacks_isempty(queue))){
	while(stack_isempty(queue->s1)!= 1){
		y = stack_pop(queue->s1);
		stack_push(queue->s2,y);
	}
	//printf("y: %d\n",y);

	// stack_push(queue->s2,x);

	 x = stack_top(queue->s2);
	//printf("x: %d\n",x);

  return x;
}

/*
 * Should remove the front element from a queue and return its value.
 *
 * Params:
 *   queue - the queue from which to dequeue a value.  May not be NULL or
 *     empty.  The function should exit the program with an error if queue is
 *     NULL or empty.
 *
 * Return:
 *   Should return the value stored at the front of the queue before that value
 *   is dequeued.
 */
int queue_from_stacks_dequeue(struct queue_from_stacks* queue) {
		assert(queue);


	int x;
	//x = stack_pop(queue->s1);

	x = stack_pop(queue->s2);

	//stack_push(queue->s2,x);
 // printf("x:%d\n",x );
  return x;
 // return 0;
}
