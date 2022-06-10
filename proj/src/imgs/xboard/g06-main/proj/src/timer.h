#pragma once

#include <stdint.h>


int (timer_subscribe_int)(uint8_t *bit_no);

int (timer_unsubscribe_int)() ;

void (timer_int_handler)();
