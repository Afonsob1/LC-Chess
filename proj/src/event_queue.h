#pragma once

#include <lcom/lcf.h>
#include <stdint.h>
#include <stdint.h>


typedef enum{
    MOUSE,
    NEW_FRAME,
    KEYBOARD,
    NO_EVENT

} EventType;

typedef union 
{
  struct packet mouse_packet;
  uint8_t scanCode;
}EventData;

typedef struct {
  EventType type;
  EventData data;
} Event;


typedef struct {

  uint32_t size; 
  uint32_t capacity; 
  Event*  array; 

} EventQueue;

/**
 * @brief Create a Event Queue object with a capacity given
 * 
 * @param capacity capacity that we want to have in the queue
 * @return EventQueue* the event queue object that we created
 */
EventQueue* createEventQueue(unsigned capacity);

/**
 * @brief Checks if the event queue is full
 * 
 * @param queue queue that we want to check if is full
 * @return true if full
 * @return false otherwise
 */
bool eventQueueIsFull(EventQueue * queue);

/**
 * @brief Checks if the the event queue if empty
 * 
 * @param queue queue that we want to check if is empty
 * @return true if is emptif empty
 * @return false otherwise
 */
bool eventQueueIsEmpty(EventQueue * queue);
/**
 * @brief frees the memory allocated to the EventQueue
 * 
 * @param queue queue whose memory we want to free
 */


void freeEventQueue(EventQueue * queue);

/**
 * @brief add a new event to the queue
 * @param queue  queue where we want to add the event
*/
void pushEvent(EventQueue * queue, Event event);

/**
 * @brief return the first element of the queue
 * @param queue  the queue
 * @return the first element of the queue
*/
Event popEvent(EventQueue * queue);

