#pragma once

#include <stdint.h>

typedef enum{
    PRESSED,
    REALEASED

} GameEvent;

typedef struct {

  uint32_t size; 
  uint32_t capacity; 
  void**  array_elements;
  GameEvent*  array_type; 

} EventQueue;


void initEventQueue(EventQueue * queue);
