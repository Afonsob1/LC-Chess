

#include "event_queue.h"

EventQueue* createEventQueue(unsigned capacity){

    EventQueue * queue = (EventQueue*) malloc(sizeof(EventQueue)); 

    queue->capacity = capacity; 
    queue->size = 0;
    queue->array = (Event*) malloc(queue->capacity * sizeof(Event)); 

    return queue;

}

bool eventQueueIsFull(EventQueue * queue) {
  return (queue->size == queue->capacity);  
} 


bool eventQueueIsEmpty(EventQueue * queue) {  
  return (queue->size == 0); 
} 

void freeEventQueue(EventQueue * queue){
    if(!queue) return;

    if(queue->array) free(queue->array);
    free(queue);
}

void pushEvent(EventQueue * queue, Event event){
    if (eventQueueIsFull(queue)) return; 
  
    queue->array[queue->size++] = event; 
    
}

Event popEvent(EventQueue * queue){
    if(eventQueueIsEmpty(queue)){ 
        Event ev;
        ev.type = NO_EVENT;
        return ev;
    } 
    return queue->array[--queue->size];
}
