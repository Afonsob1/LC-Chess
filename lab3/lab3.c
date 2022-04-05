#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include <keyboard.h>

int hook_id_keyboard=1;
int hook_id_timer=4;
int counter;
uint8_t c=0;

#define DELAY_US    20000
#define OUTPUT_BUFFER 0x60
#define STAT_REG 0x64
#define TIMER0_IRQ 0
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define DELAY_US    20000

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
  /* To be completed by the students */
    sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_EXCLUSIVE | IRQ_REENABLE,&hook_id_keyboard);
    int ipc_status,r;
    message msg;
    while(c!=0x81) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        /* To be implemented by the students */
          int irq_set=BIT(1);
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */				
                  if (msg.m_notify.interrupts & irq_set) {
                    kbc_ih();
                  }
                  break;

              default:
                  break; /* no other notifications expected: do nothing */	
          }
      }
      else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }
  sys_irqrmpolicy(&hook_id_keyboard);
  return 1;
}


int(kbd_test_poll)() {
  /* To be completed by the students */
  c=0;
  while(c!=0x81){
    uint8_t status;
    util_sys_inb(STAT_REG,&status);
    if(status&0x01 && !(status&32)){
        util_sys_inb(OUTPUT_BUFFER,&c);
      if(c==0xe0){
        uint8_t bytes[2];
        bytes[0]=c;
        util_sys_inb(OUTPUT_BUFFER,&c);
        bytes[1]=c;
        kbd_print_scancode(!(c&0x80),2,bytes);
      }
      else{
        uint8_t bytes[1];
        bytes[0]=c;
        kbd_print_scancode(!(c&0x80),1,bytes);
      }
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
  uint8_t write_command = 0x20;
  issue_command(&write_command,0);
  uint8_t previous_command_byte = write_command;
  write_command = 0x60;
  issue_command(&write_command,0x01 | previous_command_byte);
  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
    /* To be completed by the students */
    sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_EXCLUSIVE | IRQ_REENABLE,&hook_id_keyboard);
    sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&hook_id_timer);
    int ipc_status,r;
    message msg;
    counter = n*60;
    while(c!=0x81 && counter>0) { /* You may want to use a different condition */
      /* Get a request message. */
      if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
          printf("driver_receive failed with: %d", r);
          continue;
      }
      if (is_ipc_notify(ipc_status)) { /* received notification */
        /* To be implemented by the students */
          int irq_set_keyboard=BIT(1);
          int irq_set_timer=BIT(4);
          switch (_ENDPOINT_P(msg.m_source)) {
              case HARDWARE: /* hardware interrupt notification */				
                  if (msg.m_notify.interrupts & irq_set_keyboard) {
                    kbc_ih();
                    counter = n*60;
                  }
                  if(msg.m_notify.interrupts & irq_set_timer){
                    timer_int_handler();
                  }
                  break;

              default:
                  break; /* no other notifications expected: do nothing */	
          }
      }
      else { /* received a standard message, not a notification */
          /* no standard messages expected: do nothing */
      }
  }
  sys_irqrmpolicy(&hook_id_keyboard);
  sys_irqrmpolicy(&hook_id_timer);
  return 1;
}
