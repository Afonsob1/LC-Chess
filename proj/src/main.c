#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "imgs/menu.h"
#include "imgs/player_choice.h"
#include "timer.h"
#include "cursor.h"
#include "keyboard.h"

extern struct packet pp;
extern bool updateMouse;
extern int n_interrupts;
extern char * video_mem_buffer;
extern char * video_mem;
extern xpm_image_t letterImg;

int main(int argc, char* argv[]){


    lcf_set_language("EN-US");

    //lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

    init_graphics_mode(0x115);
    xpm_image_t menu;
    xpm_image_t player_choice;
    create_image(menu_xpm,&menu);
    draw_image(video_mem,menu,95,50);
    sleep(2);
    create_image(player_choice_xpm,&player_choice);

    
    Board board;
    int ipc_status, err;
    message msg;

    /* subscribe timer */
    n_interrupts = 0;
    uint8_t bit_timer;
    timer_subscribe_int(&bit_timer);
    uint8_t irq_set_timer = BIT(bit_timer);

    /* subscribe keyboard */
    uint8_t bit_kb;
    keyboard_subscribe_int(&bit_kb);
    uint8_t irq_set_kb = BIT(bit_kb);


    /* subscribe mouse */
    Cursor cursor;
    initCursor(&cursor);
    uint8_t bit_mouse;
    int irq_set_mouse;
    
    mouse_enable_data_reporting();
    mouse_subscribe_int(&bit_mouse);

    irq_set_mouse = BIT(bit_mouse);



    //initBoard(&board);


    /* draw board*/
    //drawBoard(&board);
    //copy_from_buffer();
    bool click = false;
    int count = 0;
    while(n_interrupts < 60*60){
      /* Get a request message. */
      if( (err = driver_receive(ANY, &msg, &ipc_status)) != 0 ) {
        printf("driver_receive failed with: %d", err);
        continue;
      }
      if (is_ipc_notify(ipc_status)) {
        switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE: /* hardware interrupt notification */
          
          if(msg.m_notify.interrupts & irq_set_timer){
              timer_int_handler();

              if(click){
                if (count == 0){
                  initBoard(&board);
                  count++;
                }
                else{
                vg_clear(video_mem_buffer);
                drawBoard(&board);
                updateBoard(&board);
                drawBoardPieces(&board);
                }
              } 
              else{
                vg_clear(video_mem_buffer);
                 draw_image(video_mem_buffer,player_choice,95,15);
              }
              drawCursor(video_mem_buffer,&cursor);
              copy_from_buffer();   
              
          }
          if (msg.m_notify.interrupts & irq_set_mouse) {
            mouse_ih();
            
            if (!click){
              click = cursorClickPlayer(&cursor);
            } 
            

          }
          if (msg.m_notify.interrupts & irq_set_kb) {
              keyboard_int_handler();
              draw_image(video_mem_buffer, letterImg, 0, 0);
              copy_from_buffer();
          }
          break;
        default:
          break; 
        }
      }
      else { 
      }

      if(updateMouse){
        updateCursor(&board,&cursor,&pp);
        updateMouse=false;
      }

  }

  /*unsubscribe timer*/
  timer_unsubscribe_int();



  /*unsubscribe mouse*/
  mouse_unsubscribe_int();
  mouse_disable_data_reporting();

  return return_text_mode();
}
