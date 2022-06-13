#pragma once

#include <lcom/lcf.h>


typedef struct {

  uint8_t front, rear, size; 
  unsigned capacity; 
  uint8_t * array; 

} Queue;

/**
 *  @brief crerates a queue allocating memory
 *  @param capacity capacity of the queu
 *  @return queueu created
 */
Queue * createQueue(unsigned capacity);

/**
 *  @brief checks if the queue is full
 *  @param queue queueu that we want to check if is full
 *  @return 1 if is full 0 otherwise
 */
int queueIsFull(Queue * queue);

/**
 *  @brief checks if the queue is empty
 *  @param queue queue that we want to check if is empty
 *  @return 1 if is full 0 otherwise
 */
int queueIsEmpty(Queue * queue);

/**
 *  @brief adds item to the queu
 *  @param queue queue that we want to add items to
 *  @param item item that we want to add
 */
void addToQueue(Queue * queue, uint8_t item);

/**
 *  @brief removes item from queue
 *  @param queue queue that we want to remove a item from
 *  @return item removed from queu
 */
uint8_t removeFromQueue(Queue * queue);

/**
 *  @brief gets the fron item of the queue
 *  @param queue queue that we want to get the first element 
 *  @return front item
 */
int queueFront(Queue * queue);

/**
 *  @brief gets the last element of the queue
 *  @param queue queue that we want to get the last element
 *  @return 0 if succeeded 1 otherwise
 */
int queueRear(Queue * queue);

