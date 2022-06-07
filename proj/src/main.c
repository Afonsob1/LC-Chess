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
extern bool writeToSP;



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
  
  uint32_t irq_set_sp = BIT(SP_COM1_IRQ);
  uint8_t sp_bit_no;
  if (sp_subscribe_int(&sp_bit_no) != 0) {return 1;}

  uint8_t bit_no_timer;

  timer_subscribe_int(&bit_no_timer);
  uint8_t irq_set_timer = BIT(bit_no_timer);

  addToTransmitQueue(VM_CONNECTED);
  sp_write();
  int count=0;
  while(1) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: 
            if (msg.m_notify.interrupts & irq_set_timer){
              count++;
              if(count%60==0){
                for(int i=1;i<=5;i++){
                  if(i!=4){
                    uint8_t value_read;
                    util_sys_inb(0x3F8+i,&value_read);
                    printf("Register %d: %X\n",i,value_read);
                  }
                }
                printf("\n");
              }
            }
            if (msg.m_notify.interrupts & irq_set_sp) {         
              uint32_t interruptType = 0x00;
              if (sys_inb(SP_COM1 + SP_UART_IIR, &interruptType) != 0) {return 0;}


              if (interruptType & SP_IIR_NO_PENDING_INTERRUPT)
                printf("No pending interrupt\n");

              if (interruptType & SP_IIR_RECEIVED_DATA) {
                printf("received data\n");
                sp_read();
                addToTransmitQueue(0x12);
              }
              else if (interruptType & SP_IIR_TRANSMITTER_HOLDING) {
                printf("Sending data\n");
                sp_write();
              }
              else if (interruptType & SP_IIR_RECEIVER_LINE) {
                printf("\n\nERROR BIT IS 1 ON LSR\n\n");
              }
              else if (interruptType & SP_IIR_MODEM_STATUS) {
                printf("MODEM STATUS.\n");
              }
              else if (interruptType & SP_IIR_CHAR_TIMEOUT) {
                printf("CHARACTER TIMEOUT.\n");
              }  
            }
          
					
            break;
        default:
          break; 
			    
      }
    }
  }

  addToTransmitQueue(VM_DISCONNECTED);
  timer_unsubscribe_int();
  if (sp_unsubscribe_int() != 0) {return 1;}
	return 0;
	

}
