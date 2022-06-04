#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "imgs/xboard/cursor.h"

extern struct packet pp;
extern bool updateMouse;
#include "timer.h"

extern int n_interrupts;
extern char * video_mem_buffer;
extern char * video_mem;

int main(int argc, char* argv[]){


    lcf_set_language("EN-US");

    //lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

    init_graphics_mode(INDEXED_MODE);
    Board board;
    initBoard(&board);
    int ipc_status, err;
    message msg;

    /* subscribe timer */
    n_interrupts = 0;
    uint8_t bit_timer;
    timer_subscribe_int(&bit_timer);
    uint8_t irq_set_timer = BIT(bit_timer);

    
    /* subscribe mouse */
    Cursor cursor = {0,0};
    uint8_t bit_mouse;
    int irq_set_mouse;

    mouse_enable_data_reporting();
    mouse_subscribe_int(&bit_mouse);

    irq_set_mouse = BIT(bit_mouse);

    /* mouse image*/
    xpm_image_t img_mouse;
    create_image(arrow, &img_mouse);

    /* draw board*/
    drawBoard(&board);
    copy_from_buffer();
    
    movePiece(board.board[get_position(1,7)], getScreenX(2), getScreenY(5));
      
    while(n_interrupts < 60*5){
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
              
              updateBoard(&board);
              
              drawBoardPieces(&board);
              
              copy_from_buffer(); 
              draw_image(video_mem, img_mouse, cursor.cursor_x, cursor.cursor_y);
              
          }
          if (msg.m_notify.interrupts & irq_set_mouse) {
              mouse_ih();


            }
          break;
        default:
          break; 
        }
      }
      else { 
      }

      if(updateMouse){
        /*move mouse*/
        if(cursor.cursor_x< -pp.delta_x)
          cursor.cursor_x=0;
        else if(cursor.cursor_x+pp.delta_x+11>=(int)get_h_res())
          cursor.cursor_x=get_h_res()-12;
        else
          cursor.cursor_x+=pp.delta_x;

        if(cursor.cursor_y< pp.delta_y)
          cursor.cursor_y=0;
        else if(cursor.cursor_y-pp.delta_y+19>=(int)get_v_res())
          cursor.cursor_y=get_v_res()-20;
        else
          cursor.cursor_y-=pp.delta_y;
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
