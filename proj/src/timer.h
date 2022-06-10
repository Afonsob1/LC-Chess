#pragma once

#include <stdint.h>

/**
 * @brief it subscribes the interrutpion line for timer
 * 
 */
int (timer_subscribe_int)(uint8_t *bit_no);
/**
 * @brief 
 * 
 */
int (timer_unsubscribe_int)() ;

void (timer_int_handler)();
