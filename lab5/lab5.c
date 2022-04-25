#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "videocard.h"
#include <math.h>

extern uint8_t code;

#define ESC_BREAKCODE 0x81

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


int (video_test_init)(uint16_t mode, uint8_t delay){
  init_graphics_mode(mode);
  sleep(delay);
  return vg_exit();
}

int (video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  map_vram(mode);
  init_graphics_mode(mode);
  vg_draw_rectangle(x,y,width,height,color);
  uint8_t bit_no;
  kb_subscribe_int(&bit_no);
  int irq_set=BIT(bit_no);
  int ipc_status,r;
  message msg;
  code=0;
  while( code!=ESC_BREAKCODE ) { /* You may want to use a different condition */
    /* Get a request message. */
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & irq_set) {
          kb_int_handler();
        }
        break;
      default:
        break; 
      }
    }
    else { 
    }
  }
  kb_unsubscribe_int();
  return vg_exit();
}


int (video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
  map_vram(mode);
  init_graphics_mode(mode);

  vg_draw_rectangles(mode,no_rectangles,first,step);


  printf("done drawing!\n");
  uint8_t bit_no;
  kb_subscribe_int(&bit_no);
  int irq_set=BIT(bit_no);
  int ipc_status,r;
  message msg;
  code=0;
  while( code!=ESC_BREAKCODE ) { /* You may want to use a different condition */
    /* Get a request message. */
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & irq_set) {
          kb_int_handler();
        }
        break;
      default:
        break; 
      }
    }
    else { 
    }
  }
  kb_unsubscribe_int();
  return vg_exit();
}
