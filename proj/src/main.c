#include "videocard.h"
#include "board.h"
#include "timer.h"

extern int n_interrupts;

int main(int argc, char* argv[]){

    lcf_set_language("EN-US");

    //lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}

int(proj_main_loop)(int argc, char *argv[]) {

    map_vram(INDEXED_MODE);
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


     while(n_interrupts < 60*20){
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
            board.board[0]->absolute_y += n_interrupts;
            drawBoard(&board);
            /*if(speed<0){
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
          }*/
        }
        break;
      default:
        break; 
      }
    }
    else { 
    }
  }
    printf("acabou");

    timer_unsubscribe_int();
    return return_text_mode();
}
