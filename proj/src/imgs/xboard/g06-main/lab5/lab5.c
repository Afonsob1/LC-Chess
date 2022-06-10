// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>
#include <lcom/lab2.h>

#include <stdint.h>
#include <stdio.h>

#include "videocard.h"


extern int counter;
extern uint8_t code;
extern bool done;
#define ESC_BREAKCODE 0x81

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  int err;
  if((err=set_vbe_mode(mode))!=0)
    return err;
  sleep(delay);
  return vg_exit();
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
         
  int err;

  if((err=map_vram(mode))!=0)
    return err;

  if((err=set_vbe_mode(mode))!=0)
    return err;

  if((err=vg_draw_rectangle(x,y,width,height,color))!=0)
    return err;
  
  uint8_t bit_no;

  kb_subscribe_int(&bit_no);
  int irq_set=BIT(bit_no);
  int ipc_status;
  message msg;
  code=0;


  while(code!=ESC_BREAKCODE){
    /* Get a request message. */
    if( (err = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", err);
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

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  int err;

  if((err=map_vram(mode))!=0)
    return err;

  if((err=set_vbe_mode(mode))!=0)
    return err;

  if((err=vg_draw_matrix(mode,no_rectangles,first,step))!=0)
    return err;
  
  uint8_t bit_no;

  kb_subscribe_int(&bit_no);
  int irq_set=BIT(bit_no);
  int ipc_status;
  message msg;
  code=0;


  while(code!=ESC_BREAKCODE){
    /* Get a request message. */
    if( (err = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", err);
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

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  int err;

  if((err=map_vram(INDEXED_MODE))!=0)
    return err;

  if((err=set_vbe_mode(INDEXED_MODE))!=0)
    return err;

  xpm_image_t img;
  if(xpm_load(xpm,XPM_INDEXED,&img)==NULL)
    return -1;

  vg_draw_image(x,y,img);




  uint8_t bit_no;

  kb_subscribe_int(&bit_no);
  int irq_set=BIT(bit_no);
  int ipc_status;
  message msg;
  code=0;


  while(code!=ESC_BREAKCODE){
    /* Get a request message. */
    if( (err = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", err);
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

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {

  map_vram(INDEXED_MODE);
  set_vbe_mode(INDEXED_MODE);

  xpm_image_t img;
  xpm_load(xpm,XPM_INDEXED,&img);

  int err;
  uint8_t bit_no_keyboard,bit_no_timer;

  kb_subscribe_int(&bit_no_keyboard);
  int irq_set_keyboard=BIT(bit_no_keyboard);

  timer_subscribe_int(&bit_no_timer);
  int irq_set_timer=BIT(bit_no_timer);

  int ipc_status;
  message msg;
  code=0;

  if(xi!=xf && yi!=yf)
    return vg_exit();


  vg_draw_image(xi,yi,img);

  int16_t ticks=0;

  while(code!=ESC_BREAKCODE && (xi!=xf || yi!=yf)){
    /* Get a request message. */
    if( (err = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
      printf("driver_receive failed with: %d", err);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE: /* hardware interrupt notification */
        if (msg.m_notify.interrupts & irq_set_keyboard) {
          kb_int_handler();
        }
        if(msg.m_notify.interrupts & irq_set_timer){
          timer_int_handler();
          if(counter%(60/fr_rate)==0){
            if(speed<0){
              ticks++;
              if(ticks==-speed){
                vg_clear_image(xi,yi,img);
                if(xi!=xf){
                  xi++;
                }
                else{
                  yi++;
                }
                vg_draw_image(xi,yi,img);
                ticks=0;
              }
            }
            else{
                vg_clear_image(xi,yi,img);
                if(xi!=xf){
                  xi+=speed;
                  if(xi>xf)
                    xi=xf;
                }
                else{
                  yi+=speed;
                  if(yi>yf)
                    yi=yf;
                }
                vg_draw_image(xi,yi,img);
            }
          }
        }
        break;
      default:
        break; 
      }
    }
    else { 
    }
  }

  timer_unsubscribe_int();
  kb_unsubscribe_int();
  return vg_exit();

  
}

int(video_test_controller)() {
  return 1;
}
