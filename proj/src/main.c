#include "videocard.h"
#include "board.h"
#include "mouse.h"
#include "imgs/menu.h"
#include "imgs/player_choice.h"
#include "imgs/name_choice.h"
#include "imgs/toggle_animation.h"
#include "timer.h"
#include "cursor.h"
#include "keyboard.h"
#include "serialPort.h"
#include "serialConst.h"
#include "imgs/waiting.h"

extern struct packet pp;
extern bool updateMouse;
extern int n_interrupts;
extern char * video_mem_buffer;
extern char * video_mem;
extern uint8_t code;
uint8_t name[8];
extern int player_number;
bool animation = true;
bool connected = false;

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


void name_choice_ih(GameState* gameState, InputType type){
  static xpm_image_t name_choice;
  static int l=0;
  static bool initialize_name_choice = false;
  if(!initialize_name_choice){
    create_image(name_choice_xpm,&name_choice);
    initialize_name_choice=true;
  }
  
  switch(type){
    case Timer:
      timer_int_handler();
      vg_clear(video_mem_buffer);
      draw_image(video_mem_buffer, name_choice,95,15);
      draw_name(video_mem_buffer,name,142,312,68);
      if(l>0 && name[l-1]==0x9c){
        printf("\nWAITING\n");
        addToTransmitQueue(VM_CONNECTED); 
        *gameState=WAITING;
      }
      break;
   
    case Keyboard:
      keyboard_int_handler();
      if(code & BIT(7)){  
        if (l < 9){
          if(code == 0x8e && l>=1){ //backspace and deleting letters
            name[l-1] = 0;
            l--;
          }
          else if((l<8 && code!=0x8e)){
            name[l] = code;
            printf("%c", code);
            l++;
            
          }
        }
      }
      case SerialPort:
        sp_ih();
        break;
      case Mouse:
        mouse_ih();
        break;

  }
  if(updateMouse){
      updateCursor(NULL,&cursor,&pp);
      updateMouse=false;
  }
}

 void playing_ih(GameState* gameState, InputType type){
    static chessMove move;
    static Board* board = NULL;
    static xpm_image_t toggle_animation;

    /* Init board  */
    if(board == NULL){
      /*create animation*/
      printf("Create animation\n");
      create_image(toggle_animation_xpm, &toggle_animation);
      
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
        break;
      case Timer:
        timer_int_handler();

        /* draw pieces*/
        updateBoard(board, animation);
        drawBoardPieces(board);
        
        draw_image(video_mem_buffer,toggle_animation,538,185);

        if (player_number == 1){
          draw_name(video_mem_buffer,name,63,560,20);
        } 
        else {
          draw_name(video_mem_buffer, name, 63,23,20);
        }

                  

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
        break;
      case SerialPort:
        sp_ih();
        break;
      case Keyboard:
        keyboard_int_handler();

        if(code & BIT(7)){
          if (code == 0xac){
            if (animation) animation = false;
            else animation = true;
          }
        }
        break;
   }
  if(updateMouse){
      updateCursor(board,&cursor,&pp);
      updateMouse=false;
  }
 }

void waiting_ih(GameState* gameState, InputType type){
  static xpm_image_t waiting_background;
  xpm_image_t player_choice_image;
  static bool createdBackground = false;
  static bool draw_background = true;

  static bool player_choosed = false;

  static uint8_t received_byte;


  if(!createdBackground){
    create_image((xpm_map_t)waiting_xpm, &waiting_background);
    create_image(player_choice_xpm,&player_choice_image);
    createdBackground = true;
    printf("CREATED BACKGROUND\n");
  }

  
  switch(type){
    case Timer: 
        timer_int_handler();

        received_byte=readFromQueue();

        if(connected && received_byte!=0x00){
          player_number = received_byte;
          *gameState=PLAYING;
          printf("You are player %d\n",player_number);
        }

        if(received_byte==VM_CONNECTED){
          printf("VM CONNECTED 1 \n");
          printf("MY TURN!\n");
          myTurn=true;
          *gameState=WAITING;
          draw_background = false;
          addToTransmitQueue(RECEIVE_VM_CONNECTED);

          connected = true;

          if(player_choosed){
            
          printf("Start playing");
            addToTransmitQueue((1-received_byte)+2);
            
            *gameState=PLAYING;
          }
        }
        else if(received_byte==RECEIVE_VM_CONNECTED){

          
          printf("VM CONNECTED 2 \n");
          *gameState=WAITING;
          draw_background = false;

          connected = true;
        }
        

        
        if((!connected || myTurn) && !player_choosed){
          // escolhe jogador
          
          vg_clear(video_mem_buffer);
          draw_image(video_mem_buffer,player_choice_image,95,15);
        }
        else{
          // espera
          draw_image(video_mem_buffer, waiting_background, 0 , 0);
    
        }


        
        break;

    case Mouse:
      mouse_ih();
      if(!connected || myTurn){
        if(cursorClickPlayer(&cursor)){
          player_choosed = true;
          
          if(connected){
            printf("You are player %d\n",player_number);
            addToTransmitQueue((1-player_number)+2);
          }
        }
        
      }
      break;
    case SerialPort:
      sp_ih();
      break;
    case Keyboard:
      keyboard_int_handler();
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
    case START:
      name_choice_ih(gameState, type);
      break;
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
  
  int ipc_status, r;
  message msg;


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

  /* subscribe serial port */
  uint32_t irq_set_sp = BIT(SP_COM1_IRQ);
  uint8_t sp_bit_no;
  if (sp_subscribe_int(&sp_bit_no) != 0) {return 1;}

  /* subscribe timer */   
  uint8_t bit_no_timer;
  timer_subscribe_int(&bit_no_timer);
  uint8_t irq_set_timer = BIT(bit_no_timer);
  
  

  /* Desenhar Menu*/
  xpm_image_t menu;
  create_image(menu_xpm,&menu);
  sp_clear();
  draw_image(video_mem,menu,95,50);
  sleep(2);



  GameState state = START;

  
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
            
            drawCursor(video_mem_buffer,&cursor);
            copy_from_buffer();
          }

          if (msg.m_notify.interrupts & irq_set_sp) {         
            inputHandler(&state,SerialPort);
          }

          if (msg.m_notify.interrupts & irq_set_mouse) {         
            inputHandler(&state,Mouse);
          }
          if (msg.m_notify.interrupts & irq_set_kb) {         
            inputHandler(&state,Keyboard);
          }
          break;
        default:
          break; 
			    
      }
    }
  }

  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  if (sp_unsubscribe_int() != 0) {return 1;}
  mouse_disable_data_reporting();
	return return_text_mode();
	

}
