#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

#define READBACK_CMD (BIT(7) | BIT(6))
#define PROGRAM_TIMER(n) (n<<6)
#define INIT_MODE_MASK (BIT(4)|BIT(5))
#define COUNTING_MODE_MASK 0x0E
#define BASE_MASK 0x01

int counter=0;
int hook_id=4;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  int r;
  if (freq > TIMER_FREQ || freq<19)
  return -1;

  uint8_t control;
  timer_get_conf(timer,&control);
  control &= (COUNTING_MODE_MASK | BASE_MASK);
  control |= (TIMER_LSB_MSB | PROGRAM_TIMER(timer));

  if((r=sys_outb(TIMER_CTRL,control))!=0)
    return r;

  uint16_t div = TIMER_FREQ/freq;
  uint8_t div_MSB,div_LSB;
  util_get_MSB(div,&div_MSB);
  util_get_LSB(div,&div_LSB);

  switch(timer){
    case 0:
      if((r=sys_outb(TIMER_0,div_LSB))!=0)
        return r;
      if((r=sys_outb(TIMER_0,div_MSB))!=0)
        return r;
      break;
    case 1:
      if((r=sys_outb(TIMER_1,div_LSB))!=0)
        return r;
      if((r=sys_outb(TIMER_1,div_MSB))!=0)
        return r;
      break;
    case 2:
      if((r=sys_outb(TIMER_2,div_LSB))!=0)
        return r;
      if((r=sys_outb(TIMER_2,div_MSB))!=0)
        return r;
      break;
    default:
      return -1;
      break;
  } 
  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no=hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id);
}

int (timer_unsubscribe_int)() {
  return sys_irqrmpolicy(&hook_id);
}


void (timer_int_handler)() {
  counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint8_t read_back_command = (READBACK_CMD) | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if(sys_outb(TIMER_CTRL,read_back_command)!=0)
    return -1;
  switch(timer){
    case 0:
      return util_sys_inb(TIMER_0,st);
    case 1:
      return util_sys_inb(TIMER_1,st);
    case 2:
      return util_sys_inb(TIMER_2,st);
    default:
      return -1;
  }
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val conf;
  switch(field){
    case tsf_all:
      conf.byte=st;
      break;
    case tsf_initial:
      conf.in_mode=(st&INIT_MODE_MASK)>>4;
      break;
    case tsf_mode:
      conf.count_mode=(st&COUNTING_MODE_MASK)>>1;
      if(conf.count_mode==6 || conf.count_mode==7)
        conf.count_mode-=4; // in case don't care bit is set to 1 for counting mode 2 or 3
      break;
    case tsf_base:
      conf.bcd=st&BASE_MASK;
      break;
  }
  return timer_print_config(timer,field,conf);
}
