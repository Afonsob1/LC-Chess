// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <mouse.h>
#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you

extern unsigned packet_counter;
extern unsigned timer_counter;
extern enum machine_state machineState;

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
  int ipc_status,r;
  uint8_t bit_no;
  message msg;
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no);
  uint8_t irq_set = BIT(bit_no);
  packet_counter = 0;
  while(packet_counter<cnt){
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
  }
  mouse_unsubscribe_int();
  return mouse_disable_data_reporting();
}

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status,r;
  uint8_t bit_no_mouse,bit_no_timer;
  message msg;

  if((r=mouse_enable_data_reporting())!=0)
    return r;


  if((r=timer_subscribe_int(&bit_no_timer))!=0)
    return r;

  if((r=mouse_subscribe_int(&bit_no_mouse))!=0)
    return r;





  uint8_t irq_set_mouse = BIT(bit_no_mouse);
  uint8_t irq_set_timer = BIT(bit_no_timer);
  
  while(timer_counter<=idle_time*60){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_mouse) {
            timer_counter=0;
            mouse_ih();
          }
          if (msg.m_notify.interrupts & irq_set_timer)
            timer_ih();
          break;
        default:
          break; //unexpected notification
      }
    }
    else { 
      //received an unexpected standard message,do nothing
    }
  }
  if((r=mouse_disable_data_reporting())!=0)
    return r;
  if((r=timer_unsubscribe_int())!=0)
    return r;
  return mouse_unsubscribe_int();
}

int (mouse_test_gesture)(uint8_t xlen, uint8_t tolerance) {
  int ipc_status,r;
  uint8_t bit_no;
  message msg;
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_no);
  uint8_t irq_set = BIT(bit_no);
  packet_counter = 0;
  while(machineState!=DONE){
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
  }
  mouse_unsubscribe_int();
  return mouse_disable_data_reporting();
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
