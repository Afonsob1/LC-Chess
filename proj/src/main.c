#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "cursor.h"
#include "timer.h"
#include "serialPort.h"

extern struct packet pp;
extern bool updateMouse;
extern int n_interrupts;
extern char * video_mem_buffer;
extern char * video_mem;

int main(int argc, char* argv[]){


    lcf_set_language("EN-US");

    lcf_trace_calls("/home/lcom/labs/proj/src/trace.txt");

    lcf_log_output("/home/lcom/labs/proj/src/output.txt");

    if (lcf_start(argc, argv)) return 1;

    lcf_cleanup();

    return 0;
}
#define PLAYER1

int(proj_main_loop)(int argc, char *argv[]) {
  message msg;
  int ipc_status;
  
  //Setting Serial configuration
	if (serial_set_conf() != OK) {
		printf("FAILED serial_set_conf()\n");
		return 1;
	}

  serial_write(10);

return 0;
	int serial_irq_set;
	if ((serial_irq_set = BIT(serial_subscribe_int())) < 0) {
		printf("FAILED serial_subscribe_int()\n");
		return 1;
	}

  int r;
	int gameRunning = 1;
	while (gameRunning) {
		/* Get a request message. */
		if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
			printf("driver_receive failed with: %d\n", r);
			continue;
		}
		if (is_ipc_notify(ipc_status)) { /* received notification */
			switch (_ENDPOINT_P(msg.m_source)) {
			case HARDWARE: /* hardware interrupt notification */
				if (msg.m_notify.interrupts & serial_irq_set) { /* serial interrupt */
					printf("Received serial!!!\n");
					

            // Check type of interrupt
            
            uint8_t iir = 0;
            util_sys_inb(COM1_PORT + IIR, &iir);
            if ( iir & ~IIR_NPI ) {
              if(iir & IIR_ID){
              	uint8_t received = serial_read();
                printf("Serial Interrupt: Received!!  %d\n" , received);
              }
            }


				}

				

				

				break;
			}
		} else { /* received a standard message, not a notification */
			/* no standard messages expected: do nothing */
			printf("Error: received unexpected standard message.\n");
			return 1;
		}
	}

	/* Unsubscribe All Interrupts */

	if (serial_enable_interrupts() < 0) {
		printf("FAILED serial_enbale_interrupts and the end of the game\n");
		return 1;
	}
	if (serial_unsubscribe_int() < 0) {
		printf("FAILED serial_unsubscribe_int()\n");
		return 1;
	}

  return 0;

}
