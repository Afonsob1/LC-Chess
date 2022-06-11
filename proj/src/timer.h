#pragma once

#include <stdint.h>

/**
 * @brief it subscribes the interrutpion line for timer
 * @param bit_no number of bit that will be put as one 
 */
int (timer_subscribe_int)(uint8_t *bit_no);
/**
 * @brief it unsubscribes the interrutpion line for timer
 * 
 */
int (timer_unsubscribe_int)() ;
/**
 * @brief interruption handler for all timer interruptions. it increments the counter.
 * 
 */
void (timer_int_handler)();
