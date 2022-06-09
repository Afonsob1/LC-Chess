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


int main(int argc, char* argv[]){


    lcf_set_language("EN-US");

    //lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}
#define PLAYER1

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
  
  addToTransmitQueue(VM_CONNECTED);

  

  int count=0;
  while(1) {
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
              if(count%(60*2)==0){
                if(!vmConnected){
                  received_byte=readFromQueue();
                  printf("RECEIVED BYTE %x\n",received_byte);
                  if(received_byte==VM_CONNECTED){
                    vmConnected=true;
                          printf("VM CONNECTED\n");
                    addToTransmitQueue(RECEIVE_VM_CONNECTED);
                  }
                  else if(received_byte==RECEIVE_VM_CONNECTED){
                    vmConnected=true;
                    printf("VM CONNECTED\n");
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
    if(!vmConnected){
      sp_write();
    }
  }

  addToTransmitQueue(VM_DISCONNECTED);
  timer_unsubscribe_int();
  if (sp_unsubscribe_int() != 0) {return 1;}
	return 0;
	

}
