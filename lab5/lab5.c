#include <lcom/lcf.h>
#include <lcom/lab5.h>
#include "videocard.h"
#include <math.h>

extern uint8_t code;
extern bool done_drawing;
extern int counter;
#define ESC_BREAKCODE 0x81
#define INDEXED_MODE 0x105

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
  sleep(5);
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

int (video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y){
  map_vram(INDEXED_MODE);
  init_graphics_mode(INDEXED_MODE);
  xpm_image_t img;
  xpm_load(xpm,XPM_INDEXED,&img);
  vg_draw_image(img,x,y);

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


int (video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate){
  init_graphics(xpm,xi,yi,xf,yf,speed,fr_rate);
  uint8_t bit_no_kb,bit_no_timer;
  kb_subscribe_int(&bit_no_kb);
  timer_subscribe_int(&bit_no_timer);
  int irq_set_kb=BIT(bit_no_kb);
  int irq_set_timer=BIT(bit_no_timer);

  int ipc_status,r;
  message msg;
  code=0;

  done_drawing=false;

  while( code!=ESC_BREAKCODE && !done_drawing) { /* You may want to use a different condition */
    /* Get a request message. */
    if( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & irq_set_kb) {
          kb_int_handler();
        }
        if (msg.m_notify.interrupts & irq_set_timer) {
          timer_int_handler();
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
  vg_exit(); 
  printf("%d\n",counter); 
  return 0;
}
