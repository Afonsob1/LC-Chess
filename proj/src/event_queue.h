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

EventQueue* createEventQueue(unsigned capacity);

bool eventQueueIsFull(EventQueue * queue);


bool eventQueueIsEmpty(EventQueue * queue);

void freeEventQueue(EventQueue * queue);


void pushEvent(EventQueue * queue, Event event);


Event popEvent(EventQueue * queue);

