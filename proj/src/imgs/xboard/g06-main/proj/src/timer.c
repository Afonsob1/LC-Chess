#include <lcom/lcf.h>
#include "timer.h"

#include <stdint.h>

#include "i8254.h"


#define FAIL 1
#define OK 0

static int hook_id;
int n_interrupts = 0;


bool compare_bits(uint32_t value, uint32_t active_bits){
    return (value & active_bits)==active_bits;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = TIMER0_IRQ;
  hook_id = TIMER0_IRQ;

  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);
  
  return OK;
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}

void (timer_int_handler)() {
  n_interrupts++;
}

