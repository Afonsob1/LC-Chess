#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int counter=0;
int hook_id;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  uint8_t result;
  timer_get_conf(timer,&result);
  uint8_t control = (0x03<<4) | (0x0F & result);
  control = control |  (timer<<6);
  uint8_t div = TIMER_FREQ/freq;
  sys_outb(TIMER_CTRL,control);
  switch(timer){
    case 0:
      sys_outb(TIMER_0,div);
      break;
    case 1:
      sys_outb(TIMER_1,div);
      break;
    case 2:
      sys_outb(TIMER_2,div);
      break;
    default:
      return -1;
  }
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  counter = 60*(*bit_no);
  hook_id=4;
  *bit_no=hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id);
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  return sys_irqrmpolicy(&hook_id);
}


void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  uint8_t control = TIMER_RB_SEL(timer) | TIMER_RB_COUNT_ | 0xC0;
  sys_outb(TIMER_CTRL,control);
  int port;
  switch(timer){
    case 0:
      port = TIMER_0;
      break;
    case 1:
      port = TIMER_1;
      break;
    case 2:
      port = TIMER_2;
      break;
  } 
  util_sys_inb(port,st);
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  /* To be implemented by the students */
  union timer_status_field_val aux;
  uint8_t initial_mode = (st&0x30)>>4;
  uint8_t programmed_mode = (st & 0x0E)>>1;
  if(programmed_mode==6 || programmed_mode==7)
    programmed_mode-=4;
  uint8_t base = (st&0x01);
  switch(field){
    case tsf_all:
      aux.byte=st;
      break;
    case tsf_initial:
      aux.in_mode=initial_mode;
      break;
    case tsf_mode:
      aux.count_mode=programmed_mode;
      break;
    case tsf_base:
      aux.bcd=base;
      break;
  }
  timer_print_config(timer,field,aux);
  return 0;
}
