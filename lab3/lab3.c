#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include <keyboard.h>

extern int counter;
extern uint8_t code;

#define DELAY_US    20000
#define OUTPUT_BUFFER 0x60
#define STAT_REG 0x64
#define ESC_BREAKCODE 0x81
#define ENABLE_KBD_INT BIT(0)

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}



int(kbd_test_scan)() {
  int ipc_status,r;
  uint8_t bit_no;
  message msg;
  if((r=keyboard_subscribe_int(&bit_no))!=0)
    return r;
  uint8_t irq_set = BIT(bit_no);
  while(code!=ESC_BREAKCODE){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set) {
            if((r=keyboard_int_handler())!=0)
              printf("keyboard interrupt handler failed with: %d", r);
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
  return keyboard_unsubscribe_int();
}


int(kbd_test_poll)() {
  uint8_t c='\0';
  while(c!=ESC_BREAKCODE){
    bool valid_values=false;
    int r;
    uint8_t kbc_state;
    if((r=util_sys_inb(STAT_REG,&kbc_state))!=0){
      printf("util_sys_inb failed with: %d", r);
      continue;
    }
    if((kbc_state&OBF_FULL) && !(kbc_state & (PAR_ERR|TIMEOUT_ERR|AUX))){
      valid_values=true;
    }
    if((r=util_sys_inb(OUT_BUF,&c))!=0){
      printf("util_sys_inb failed with: %d", r);
      continue;
    }
    if(c==TWO_BYTES && valid_values){
      uint8_t bytes[2];
      bytes[0]=c;
      if((r=util_sys_inb(OUT_BUF,&c))!=0){
        printf("util_sys_inb failed with: %d", r);
        continue;
      }
      bytes[1]=c;
      if((r=kbd_print_scancode(!(c&BREAK_CODE),2,bytes))!=0){
        printf("kbd_print_scancode failed with: %d", r);
        continue;
      }
    }
    else if (valid_values){
      uint8_t bytes[1];
      bytes[0]=c;
      if((r=kbd_print_scancode(!(c&BREAK_CODE),1,bytes))!=0){
        printf("kbd_print_scancode failed with: %d", r);
        continue;
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  uint8_t command_byte;
  int r;
  if((r=read_command_byte(&command_byte))!=0){
    return r;
  }
  command_byte|=ENABLE_KBD_INT;
  return write_command_byte(command_byte);
}

int(kbd_test_timed_scan)(uint8_t n) {
  int ipc_status,r;
  uint8_t bit_no_kbc,bit_no_timer;
  message msg;

  if((r=keyboard_subscribe_int(&bit_no_kbc))!=0)
    return r;

  if((r=timer_subscribe_int(&bit_no_timer))!=0)
    return r;

  uint8_t irq_set_kbc = BIT(bit_no_kbc);
  uint8_t irq_set_timer = BIT(bit_no_timer);
  
  while(code!=ESC_BREAKCODE && counter<=n*60){
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: //hardware interrupt notification
          if (msg.m_notify.interrupts & irq_set_kbc) {
            counter=0;
            if((r=keyboard_int_handler())!=0)
              printf("keyboard interrupt handler failed with: %d", r);
          }
          if (msg.m_notify.interrupts & irq_set_timer)
            timer_int_handler();
          break;
        default:
          break; //unexpected notification
      }
    }
    else { 
      //received an unexpected standard message,do nothing
    }
  }

  return keyboard_unsubscribe_int() && timer_unsubscribe_int();
}
