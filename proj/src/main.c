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
extern bool endGame;
uint8_t name[8];
extern int player_number;
bool animation = true;
bool connected = false;
bool won;

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
  CHOOSING_PLAYER,
  WAIT_CHOOSING_PLAYER,
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
  static bool redraw = true;
  static char* background_video_mem = NULL;

  if(!initialize_name_choice){
    create_image(name_choice_xpm,&name_choice);
    initialize_name_choice=true;
  }

  if(background_video_mem == NULL){
    map_vram(&background_video_mem, 2, 0x115);


    vg_clear(background_video_mem);
    draw_image(background_video_mem, name_choice,95,15);
    draw_name(background_video_mem,name,142,312,68);
    redraw = false;
  }
  
  switch(type){
    case Timer:
      timer_int_handler();
      
      if(redraw){
        vg_clear(background_video_mem);
        draw_image(background_video_mem, name_choice,95,15);
        draw_name(background_video_mem,name,142,312,68);
        redraw = false;
      }

      copy_buffers(video_mem_buffer, background_video_mem);
      
      if(l>0 && name[l-1]==0x9c){
        printf("\nWAITING\n");
        sp_clear(); //clear vm connected received while writing name if it received
        addToTransmitQueue(VM_CONNECTED); 
        *gameState=WAITING;
        free(background_video_mem);
        background_video_mem=NULL;
      }
      break;
   
    case Keyboard:
      keyboard_int_handler();
      if(code & BIT(7)){  
        redraw = true;
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

      draw_image(board->mem_board,toggle_animation,538,185);

      
    }
    uint8_t received_byte;


  if(endGame){
    won=true;
    *gameState=EXIT;
  }
   switch(type){
      case Mouse:
        mouse_ih();
        break;
      case Timer:
        timer_int_handler();

        /* draw pieces*/
        updateBoard(board, animation);
        drawBoardPieces(board);
        

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



            printf("Received position\n Old position %d:%d \n New position %d:%d\n",move.old_col,move.old_row,move.new_col,move.new_row);
            if(board->board[move.new_row*8+move.new_col] && (board->board[move.new_row*8+move.new_col]->type==b_king || board->board[move.new_row*8+move.new_col]->type==w_king)){
                won=false;

                printf("END GAME\n");
                *gameState=EXIT;
            }
            printf("Moving %d %d %d %d\n",move.old_col,move.old_row,move.new_col,move.new_row);
            
            movePiece(board->board[move.old_row*8+move.old_col],getScreenX(move.new_col),getScreenY(move.new_row));
            board->board[move.new_row*8+move.new_col]=board->board[move.old_row*8+move.old_col];
            board->board[move.old_row*8+move.old_col]=NULL;

            
            printf("END Moving\n");
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
  if(*gameState == EXIT){
      printf("GO EXIT\n");
  }
 }

void waiting_choosing_ih(GameState* gameState, InputType type){
  static xpm_image_t background;
  static bool createdBackground = false;
  uint8_t received_byte;
  static char* background_video_mem = NULL;

  if(!createdBackground){
    create_image((xpm_map_t)waiting_xpm, &background);
    createdBackground = true;
    printf("CREATED BACKGROUND");
    map_vram(&background_video_mem, 2, 0x115);
    vg_clear(background_video_mem);
    draw_image(background_video_mem, background, 0, 0);
  }

  switch(type){
    case Timer: 
        timer_int_handler();
        
        
        copy_buffers(video_mem_buffer, background_video_mem);
        sp_read();
        received_byte=readFromQueue();
        if(received_byte!=0){
          myTurn=(received_byte==1);
          player_number=received_byte;
          *gameState=PLAYING;
          free(background_video_mem);
          background_video_mem=NULL;
        }
        break;

    case Mouse:
      mouse_ih();
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


void waiting_ih(GameState* gameState, InputType type){
  static xpm_image_t background;
  static bool createdBackground = false;
  static char* background_video_mem = NULL;
  uint8_t received_byte;
  if(!createdBackground){
    create_image((xpm_map_t)waiting_xpm, &background);
    createdBackground = true;
    printf("CREATED BACKGROUND");
  }

  if(background_video_mem==NULL){
    
    map_vram(&background_video_mem, 2, 0x115);
    vg_clear(background_video_mem);
    draw_image(background_video_mem, background, 0, 0);
    
  }
  
  switch(type){
    case Timer: 
        timer_int_handler();

        copy_buffers(video_mem_buffer, background_video_mem);

        received_byte=readFromQueue();

        if(received_byte==VM_CONNECTED){
          printf("VM CONNECTED 1 \n");
          *gameState=CHOOSING_PLAYER;
          addToTransmitQueue(RECEIVE_VM_CONNECTED);
        }
        else if(received_byte==RECEIVE_VM_CONNECTED){
          printf("VM CONNECTED 2 \n");
          *gameState=WAIT_CHOOSING_PLAYER;
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


void choosing_ih(GameState* gameState, InputType type){
  static xpm_image_t player_choice_image;
  static bool created_player_choice_image = false;
  static bool player_choosed = false;
  static char* background_video_mem = NULL;


  if(!created_player_choice_image){
    create_image(player_choice_xpm, &player_choice_image);
    created_player_choice_image=true;
  }

  if(background_video_mem==NULL){
    map_vram(&background_video_mem, 2, 0x115);
    
    vg_clear(background_video_mem);
    draw_image(background_video_mem, player_choice_image,95,15);
  }

  
  switch(type){
    case Timer: 
        timer_int_handler();
        copy_buffers(video_mem_buffer, background_video_mem);
        break;

    case Mouse:
      mouse_ih();
      
      if(cursorClickPlayer(&cursor)){
        player_choosed = true;
        *gameState = PLAYING;
        addToTransmitQueue((1-player_number)+2);
        myTurn=(player_number==1);

        free(background_video_mem);
        background_video_mem=NULL;
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
      case CHOOSING_PLAYER:
      choosing_ih(gameState, type);
      break;
      
      case WAIT_CHOOSING_PLAYER:
      waiting_choosing_ih(gameState, type);

      break;
    default:
      break;
  }

  sp_write();
  sp_read();
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

  printf("AT THE END");
  if(won)
    sp_emptyTransmitQueue(); //make sure that you send the winning move
  
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  
  printf("un sub sp");
  sp_unsubscribe_int();
  
  printf("un  mmouse sidable");
  mouse_disable_data_reporting();
  mouse_unsubscribe_int();
	return_text_mode();
  if(won){
    printf("You won!\n");
  }
  else{
    printf("You lost!\n");
  }
	
  return 0;
}
