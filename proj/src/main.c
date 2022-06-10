#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "cursor.h"
#include "timer.h"
#include "serialPort.h"
#include "serialConst.h"
#include "imgs/waiting.h"

extern struct packet pp;
extern bool updateMouse;
extern int n_interrupts;
extern char * video_mem_buffer;
extern char * video_mem;
extern bool myTurn;


int byteOfMove=0;
Cursor cursor;

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
  WAITING,
  PLAYING,
  EXIT
}GameState;

typedef enum {
  Keyboard,
  SerialPort,
  Timer,
  Mouse
}InputType;


bool white=false;


 void playing_ih(GameState* gameState, InputType type){
    static chessMove move;
    static Board* board = NULL;
    if(board == NULL){
      printf("Init board\n");
      board = malloc(sizeof(Board));
      initBoard(board);
      printf("Draw board\n");
      /* draw board*/
      drawBoard(board);
      printf("Copy form buffer\n");
      
    }
    uint8_t received_byte;


   switch(type){
      case Mouse:
        mouse_ih();
      case Timer:
        timer_int_handler();

        /* draw pieces*/
        updateBoard(board);
        drawBoardPieces(board);
        drawCursor(video_mem_buffer,&cursor);

        /* Serial Port Comunication*/
        received_byte=readFromQueue();
        if(received_byte==BEGIN_MESSAGE){
          byteOfMove=0;
        }
        else if(received_byte==YOUR_TURN){
          myTurn=true;
          printf("MY TURN!\n");
        }
        else if(received_byte!=0){
          if(byteOfMove==0){
            move.old_col=(received_byte&COL_MASK)>>3;
            move.old_row=received_byte&ROW_MASK;
            byteOfMove++;
          }
          else{
            move.new_col=(received_byte&COL_MASK)>>3;
            move.new_row=received_byte&ROW_MASK;
            printf("Received the following position:\nOld position: %d:%d\nNew position: %d:%d\n",move.old_col,move.old_row,move.new_col,move.new_row);
            movePiece(board->board[move.old_row*8+move.old_col],getScreenX(move.new_col),getScreenY(move.new_row));
          }
        }
      case SerialPort:
        sp_ih();
        break;
      default:
        break;
   }
  if(updateMouse){
      updateCursor(board,&cursor,&pp);
      updateMouse=false;
  }
 }

void waiting_ih(GameState* gameState, InputType type){
  static xpm_image_t background;
  static bool createdBackground = false;
  static bool draw_background = true;
  uint8_t received_byte;
  if(!createdBackground){
    create_image((xpm_map_t)waiting_xpm, &background);
    createdBackground = true;
    printf("CREATED BACKGROUND");
  }

  if(draw_background){
    
    printf("DRAW BACKGROUND");
    draw_image(video_mem_buffer, background, 0, 0);
    draw_background = false;
  }
  switch(type){
    case Timer: 
        timer_int_handler();
  
        received_byte=readFromQueue();
        if(received_byte==VM_CONNECTED){
          printf("VM CONNECTED 1 \n");
          *gameState=PLAYING;
          draw_background = false;
          addToTransmitQueue(RECEIVE_VM_CONNECTED);
        }
        else if(received_byte==RECEIVE_VM_CONNECTED){
          printf("VM CONNECTED 2 \n");
          *gameState=PLAYING;
          draw_background = false;
          myTurn=true;
          printf("MY TURN!\n");
        }
        
        break;

    case Mouse:
      mouse_ih();
      break;
    case SerialPort:
      sp_ih();
      break;
    default:
      break;
    
   }
  if(updateMouse){
      updateCursor(NULL,&cursor,&pp);
      updateMouse=false;
  }
}

void inputHandler(GameState* gameState, InputType type){

  switch (*gameState)
  {
    case WAITING:
      waiting_ih(gameState, type);
      break;
    case PLAYING:
      playing_ih(gameState, type);
      break;
    default:
      break;
  }

  sp_write();
}




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


  message msg;
  int ipc_status, r;

  uint32_t irq_set_sp = BIT(SP_COM1_IRQ);
  uint8_t sp_bit_no;
  if (sp_subscribe_int(&sp_bit_no) != 0) {return 1;}

  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer);
  uint8_t irq_set_timer = BIT(bit_no_timer);
  
  
  sp_clear();

  /* subscribe mouse */
  uint8_t bit_mouse;
  int irq_set_mouse;
    
  mouse_enable_data_reporting();
  mouse_subscribe_int(&bit_mouse);
  irq_set_mouse = BIT(bit_mouse);


  GameState state = WAITING;

  addToTransmitQueue(VM_CONNECTED);
  initCursor(&cursor);
  
  while(state!=EXIT) {
    if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
      printf("drive && n_interrupts/60<0r_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
          case HARDWARE: 
            if (msg.m_notify.interrupts & irq_set_timer) {    
              inputHandler(&state,Timer);
              copy_from_buffer();
            }

            if (msg.m_notify.interrupts & irq_set_sp) {         
              inputHandler(&state,SerialPort);
            }

            if (msg.m_notify.interrupts & irq_set_mouse) {         
              inputHandler(&state,Mouse);
            }
					
            break;
        default:
          break; 
			    
      }
    }
  }
  timer_unsubscribe_int();
  if (sp_unsubscribe_int() != 0) {return 1;}
  mouse_disable_data_reporting();
	return return_text_mode();
	

}
