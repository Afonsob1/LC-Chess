#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "imgs/xboard/cursor.h"

extern struct packet pp;
extern bool updateMouse;

int main(int argc, char* argv[]){

    lcf_set_language("EN-US");

    lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

Cursor cursor = {0,0};
    
map_vram(INDEXED_MODE);
init_graphics_mode(INDEXED_MODE);

Board board;
initBoard(&board);



drawBoard(&board);
      

  uint8_t bit_aux;
  int irq_set_aux,ipc_status,r;
  message msg;

  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_aux);

  irq_set_aux = BIT(bit_aux);

  xpm_image_t img;

  create_image(arrow,&img);
  vg_draw_image(img,cursor.cursor_x,cursor.cursor_y);

  while(true){
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
          break;
        default:
          break; //unexpected notification
      }
    }
    else { 
      //received an unexpected standard message,do nothing
    }
    if(updateMouse){
      vg_clear_image(img,cursor.cursor_x,cursor.cursor_y);
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
      vg_draw_mouse(img,cursor.cursor_x,cursor.cursor_y);
    }
  }
  mouse_unsubscribe_int();
  mouse_disable_data_reporting();
  return return_text_mode();
}
