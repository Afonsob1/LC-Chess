// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include "mouse.h"
#include <stdint.h>
#include <stdio.h>

extern uint32_t count;
extern bool update_count;
int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no;
  int irq_set,ipc_status,r;
  message msg;

  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no);

  irq_set = BIT(bit_no);

  while(count<cnt){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) {
            mouse_ih();
          }
          break;
        default:
          break; //unexpected notification
      }
    }
    else { 
      //received an unexpected standard message,do nothing
    }
    if(update_count){
      count++;
      update_count=false;
    }

  }
  mouse_unsubscribe_int();
  return mouse_disable_data_reporting();

}


int (mouse_test_async)(uint8_t idle_time) {
  uint8_t bit_no_aux,bit_no_timer;
  int irq_set_aux,irq_set_timer,ipc_status,r;
  message msg;

  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no_aux);

  irq_set_aux = BIT(bit_no_aux);

  timer_subscribe_int(&bit_no_timer);

  irq_set_timer = BIT(bit_no_timer);

  while(count<idle_time*60){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_aux) {
            mouse_ih();
          }
          if (msg.m_notify.interrupts & irq_set_timer) {
            timer_int_handler();
          }
          break;
        default:
          break; //unexpected notification
      }
    }
    else { 
      //received an unexpected standard message,do nothing
    }
    if(update_count){
      count=0;
      update_count=false;
    }

  }
  mouse_unsubscribe_int();
  timer_unsubscribe_int();
  return mouse_disable_data_reporting();
}

int (mouse_test_gesture)(uint8_t x_len,uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
