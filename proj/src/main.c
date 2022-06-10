#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "cursor.h"
#include "timer.h"
#include "serialPort.h"
#include "serialConst.h"

extern struct packet pp;
extern bool updateMouse;
extern int n_interrupts;
extern char * video_mem_buffer;
extern char * video_mem;
bool vmConnected=false;
#define YOUR_TURN 0xF8
#define BEGIN_MESSAGE 0xF9
#define COL_MASK 0x38
#define ROW_MASK 0x07
typedef struct{
  uint8_t old_row;
  uint8_t old_col;
  uint8_t new_row;
  uint8_t new_col;
}chessMove;

typedef enum {
  START,
  PLAYING,
  EXIT
}gameStates;


int main(int argc, char* argv[]){


    lcf_set_language("EN-US");

    //lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}


int(proj_main_loop)(int argc, char *argv[]) {
  message msg;
  int ipc_status, r;
  sp_clear();
  uint32_t irq_set_sp = BIT(SP_COM1_IRQ);
  uint8_t sp_bit_no;
  if (sp_subscribe_int(&sp_bit_no) != 0) {return 1;}

  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer);
  uint8_t irq_set_timer = BIT(bit_no_timer);

  sp_clear();
  int count=0;
  int byteOfMove=0;
  bool myTurn=false;
  gameStates state=START;
  chessMove move;
  addToTransmitQueue(VM_CONNECTED);
  while(state!=EXIT) {
    uint8_t received_byte;
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: 
            if (msg.m_notify.interrupts & irq_set_timer) {         
              count++;
              if(count%60*10==0){
                received_byte=readFromQueue();
                if(received_byte==VM_CONNECTED){
                  state=PLAYING;
                  addToTransmitQueue(RECEIVE_VM_CONNECTED);
                }
                else if(received_byte==RECEIVE_VM_CONNECTED){
                  state=PLAYING;
                  myTurn=true;
                }
                else if(received_byte==VM_DISCONNECTED){
                  state=EXIT;
                }
                else if(received_byte==BEGIN_MESSAGE){
                  byteOfMove=0;
                }
                else if(received_byte==YOUR_TURN){
                  myTurn=true;
                }
                else if(received_byte!=0 && state==PLAYING){
                  if(byteOfMove==0){
                    move.old_col=(received_byte&COL_MASK)>>3;
                    move.old_row=received_byte&ROW_MASK;
                    byteOfMove++;
                  }
                  else{
                    move.new_col=(received_byte&COL_MASK)>>3;
                    move.new_row=received_byte&ROW_MASK;
                    printf("Received the following position:\nOld position: %d:%d\nNew position: %d:%d\n",move.old_col,move.old_row,move.new_col,move.new_row);
                  }
                }
              }
            }

            if (msg.m_notify.interrupts & irq_set_sp) {         
              sp_ih();
            }
          
					
            break;
        default:
          break; 
			    
      }
    }
    if(myTurn){
      srand(time(0));
      move.new_col = rand()%8;
      move.new_row = rand()%8;
      move.old_col = rand()%8;
      move.old_row = rand()%8;
      uint8_t old_position = (move.old_col<<3)|move.old_row;
      uint8_t new_position = (move.new_col<<3)|move.new_row;
      addToTransmitQueue(BEGIN_MESSAGE);
      addToTransmitQueue(old_position);
      addToTransmitQueue(new_position);
      addToTransmitQueue(YOUR_TURN);
      printf("Sent the following position:\nOld position: %d:%d\nNew position: %d:%d\n",move.old_col,move.old_row,move.new_col,move.new_row);
      myTurn=false;
    }
    sp_write();
  }
  addToTransmitQueue(VM_DISCONNECTED);
  sp_write();
  timer_unsubscribe_int();
  if (sp_unsubscribe_int() != 0) {return 1;}
	return 0;
	

}
