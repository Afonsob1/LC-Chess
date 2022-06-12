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
#include "event_queue.h"
#include "letter.h"

#define MAX_NAME_SIZE 8

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


typedef enum {
  NOT_DECIDED,
  BLACK,
  WHITE
}Player;

/* keyboard global variables*/
bool readKey;
extern uint8_t scancode;


/* mouse global variables*/
extern struct packet pp;
extern bool updateMouse;

/* mouse global variables*/
extern int n_interrupts;


/* videocard global variables*/
extern char * video_mem_buffer;
extern char * video_mem;


/* game global variables */
Cursor cursor;
Player player;    

uint8_t name[MAX_NAME_SIZE+1];

bool animation = true;
bool connected = false;

bool myTurn=false;
bool endGame=false;
bool won;



void name_choice_ih(GameState* gameState, Event* event){

  static int name_size=0;

  /* draw background variables*/
  static xpm_image_t name_choice;
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
  
  switch(event->type){
    case NEW_FRAME:
      
      if(redraw){
        vg_clear(background_video_mem);
        draw_image(background_video_mem, name_choice,95,15);
        draw_name(background_video_mem,name,142,312,68);
        redraw = false;
      }

      copy_buffers(video_mem_buffer, background_video_mem);
      
      break;
   
    case KEYBOARD:
      if(event->data.scanCode & BIT(7)){
        printf("INICIO - %x \n", event->data.scanCode);
        redraw = true;
      
        uint8_t code = event->data.scanCode ;
        if (code == BACK_SPACE && name_size>0)
        {
          name_size--;
          name[name_size] = 0;
        }else if(code == ENTER && name_size>0){
          name[name_size] = 0;

          printf("\nWAITING \n");
          sp_clear(); //clear vm connected received while writing name if it received
          addToTransmitQueue(VM_CONNECTED);   // TRY TO CONNECT TO OTHER PLAYER
          *gameState=WAITING;

          /* free background_video_mem */
          free(background_video_mem);
          background_video_mem=NULL;
        }else if(code!=BACK_SPACE && name_size<MAX_NAME_SIZE){
          
          printf("entrou - %x \n", event->data.scanCode);
          name[name_size++] = code;
          printf("%c", code);
          
        }

      }
      break;
      default:
      break;
  }
}

 void playing_ih(GameState* gameState, Event* event){
    static chessMove move;
    static Board* board = NULL;
    static xpm_image_t toggle_animation;
    static int byteOfMove=0;

    
    static Piece* dragging_piece= NULL; // store the piece that user is selecting
    static unsigned inicialPieceRow = 0; // store the location of the piece before it was pressed
    static unsigned inicialPieceCol = 0; // store the location of the piece before it was pressed

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

   switch(event->type){
      case NEW_FRAME:

        /* draw pieces*/
        updateBoard(board, animation);
        drawBoardPieces(board);
        

        if (player == WHITE){
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
      case KEYBOARD:

        if(event->data.scanCode & BIT(7)){
          if (event->data.scanCode == 0xac){
            if (animation) animation = false;
            else animation = true;
          }
        }
        break;

      case MOUSE:
        
        if(dragging_piece){
          // change piceposition to cursor position
          setPiecePosition(dragging_piece,cursor.x-29,cursor.y-29); //-29 so cursor is in the center of the image
        }else if (cursor.pressed && myTurn){
          // se user clicar e for a minha vez de jogar
          int y = getBoardY(cursor.y);
          int x = getBoardX(cursor.x);

          // verifica se clicou em cima do tabuleiro
          if( y>=0 && y<8 && x >=0 && x<8){
                
            // se existir uma peca valida nessa posicao  
            Piece * piece = board->board[y*8+x];
            if((piece!=NULL) && (
                  (piece->type>5 && player == WHITE) || //player 1 can only press white pieces
                  (piece->type<=5 && player == BLACK))){ //player 2 can only press black pieces

                    dragging_piece = board->board[getBoardY(cursor.y)*8+getBoardX(cursor.x)];
                    changePiecePriority(board, dragging_piece, true);
                    
                    inicialPieceCol = getBoardX(cursor.x);
                    inicialPieceRow = getBoardY(cursor.y);

                    
                    printf(" ****** Possible solutions\n");
                    possiblePosition(board, dragging_piece, inicialPieceCol, inicialPieceRow);
                }
            }

        }
        
        if(cursor.released && dragging_piece){
          

          
          changePiecePriority(board, dragging_piece, false);


          printf(" ****** RELEASED\n");
          int y = getBoardY(cursor.y);
          int x = getBoardX(cursor.x);

          uint32_t orange = 0xed820e;
          printf("%d\n",board->mem_board[3*(getScreenY(y)*get_h_res()+getScreenX(x))]);
          if( y>=0 && y<8 && x >=0 && x<8 && memcmp(&board->mem_board[3*(getScreenY(y)*get_h_res()+getScreenX(x))],&orange,3)==0){
            Piece* newPosition = board->board[y*8+x];
            if(newPosition != NULL){
              
              bool sameTeam = (dragging_piece->type>5 && newPosition->type>5) || (dragging_piece->type<=5 && newPosition->type<=5);

              if(!sameTeam){
                // comer
                printf(" ****** not same team\n");
                setPiecePosition(dragging_piece,getScreenX(inicialPieceCol),getScreenY(inicialPieceRow));
                movePiece(dragging_piece, getScreenX(x), getScreenY(y));
                
                printf(" ****** end same team\n");

                if(newPosition->type==b_king || newPosition->type==w_king ){
                    endGame=true;
                }
                
                board->board[inicialPieceRow*8+inicialPieceCol]=NULL;
                board->board[y*8+x] = dragging_piece;
                

                printf(" ****** WTF\n");

                uint8_t old_pos = (inicialPieceCol <<3) | inicialPieceRow;
                uint8_t new_pos = (x << 3) | y;
                addToTransmitQueue(BEGIN_MESSAGE);
                addToTransmitQueue(old_pos);
                addToTransmitQueue(new_pos);
                addToTransmitQueue(YOUR_TURN);
                myTurn=false;

              }else{
                //nao pode ir para esta posicao voltar a meter a peca na posicao inicial
                printf(" ****** setPiecePosition\n");
                setPiecePosition(dragging_piece,getScreenX(inicialPieceCol),getScreenY(inicialPieceRow));
              }
              
            }else{
              printf(" ****** position livre\n");
              setPiecePosition(dragging_piece,getScreenX(inicialPieceCol),getScreenY(inicialPieceRow));
              movePiece(dragging_piece, getScreenX(x), getScreenY(y));
              board->board[inicialPieceRow*8+inicialPieceCol]=NULL;

              printf(" ****** mokito\n");
              
              
              board->board[y*8+x] = dragging_piece;

              printf(" ****** fu\n");
              uint8_t old_pos = (inicialPieceCol <<3) | inicialPieceRow;
              uint8_t new_pos = (x << 3) | y;
              addToTransmitQueue(BEGIN_MESSAGE);
              addToTransmitQueue(old_pos);
              addToTransmitQueue(new_pos);
              addToTransmitQueue(YOUR_TURN);
              myTurn=false;
              printf(" ****** end turn\n");
            }
          
          }else{
            printf(" ****** setPiecePosition\n");
            setPiecePosition(dragging_piece,getScreenX(inicialPieceCol),getScreenY(inicialPieceRow));
                    
          }
          dragging_piece=NULL;
          drawBoard(board); // redraw board to clear possible positions
        }

        

        break;
      default:
      break;
   }

 }


void waiting_choosing_ih(GameState* gameState, Event* event){
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

  switch(event->type){
    case NEW_FRAME: 
        
        copy_buffers(video_mem_buffer, background_video_mem);
        
        sp_read();
        received_byte=readFromQueue();  // try to receive my color piece
        if(received_byte!=0){

          if(received_byte==WHITE){
            myTurn=true; // if my pieces are white its my turn
            player=WHITE;
          }else{
            myTurn=false; 
            player=BLACK;
          }
          *gameState=PLAYING;

          /* free background_video_mem */
          free(background_video_mem);
          background_video_mem=NULL;
        }
        break;
    default:
    break;

   }
}

void waiting_ih(GameState* gameState, Event* event){
  static xpm_image_t background;
  static bool createdBackground = false;
  static char* background_video_mem = NULL;
  uint8_t received_byte;

  if(!createdBackground){
    create_image((xpm_map_t)waiting_xpm, &background);
    createdBackground = true;
    printf("CREATED BACKGROUND");
  }


  /* create waiting background and draw in background_video_mem */
  if(background_video_mem==NULL){
    map_vram(&background_video_mem, 2, 0x115);
    vg_clear(background_video_mem);
    draw_image(background_video_mem, background, 0, 0);
  }
  
  switch(event->type){
    case NEW_FRAME: 

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
    default:
      break;

   }
}



bool cursorClickPlayer(Cursor *cursor){
    //player 2
    if((cursor->x > 307 && cursor->x < 466  && cursor->y > 62 && cursor->y <104) && cursor->pressed){
        player = BLACK;
        return true;
    }
    //player 1
    if ((cursor->x > 307 && cursor->x < 466 && cursor->y > 517 && cursor->y <558) &&  cursor->pressed){
        player = WHITE;
        return true;
    }
    return false;
}

void choosing_ih(GameState* gameState, Event* event){
  static xpm_image_t player_choice_image;
  static bool created_player_choice_image = false;
  static bool player_choosed = false;
  static char* background_video_mem = NULL;


  if(!created_player_choice_image){
    create_image(player_choice_xpm, &player_choice_image);
    created_player_choice_image=true;
  }

  /* create player_choice_image and draw in background_video_mem */
  if(background_video_mem==NULL){
    map_vram(&background_video_mem, 2, 0x115);
    
    vg_clear(background_video_mem);
    draw_image(background_video_mem, player_choice_image,95,15);
  }

  
  switch(event->type){
    case NEW_FRAME: 
        copy_buffers(video_mem_buffer, background_video_mem);
        break;

    case MOUSE:
      
      if(cursorClickPlayer(&cursor)){
        player_choosed = true;
        *gameState = PLAYING;

        /* Send to other player if he is black or white*/
        addToTransmitQueue((player==WHITE? BLACK : WHITE));

        /* Is my turn if i am WHITE pieces*/
        myTurn=(player==WHITE);

        /* free background_video_mem*/
        free(background_video_mem);
        background_video_mem=NULL;
      }
        
      break;
    default:
    break;
   }
   
   
  
}



void inputHandler(GameState* gameState, Event* event){

  switch (*gameState)
  {
    case START:
      name_choice_ih(gameState, event);
      break;
    case WAITING:
      waiting_ih(gameState, event);
      break;
    case PLAYING:
      playing_ih(gameState, event);
      break;
      case CHOOSING_PLAYER:
      choosing_ih(gameState, event);
      break;
      
      case WAIT_CHOOSING_PLAYER:
      waiting_choosing_ih(gameState, event);

      break;
    default:
      break;
  }

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

  // init letters
  initLetter();


  /* Keyboard Variables*/
  uint8_t scancode_bytes[2];
  uint8_t counter = 0;

  /* subscribe keyboard */
  uint8_t bit_kb;
  keyboard_subscribe_int(&bit_kb);
  uint8_t irq_set_kb = BIT(bit_kb);


  /* subscribe mouse */
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
  vg_clear(video_mem);
  draw_image(video_mem,menu,95,50);
  sleep(2);


  /* Game Variables*/
  GameState state = START;
  EventQueue * eventQueue = createEventQueue(100);

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
            
            timer_int_handler(); 
            Event ev;
            ev.type = NEW_FRAME;
            pushEvent(eventQueue, ev);
          }

          if (msg.m_notify.interrupts & irq_set_sp) {
            sp_ih();       
          }

          if (msg.m_notify.interrupts & irq_set_mouse) {    
            mouse_ih();
      
            if(updateMouse){
              Event ev;
              ev.type = MOUSE;
              ev.data.mouse_packet = pp;
              pushEvent(eventQueue, ev);
              updateMouse = false;
            }
            
          }
          if (msg.m_notify.interrupts & irq_set_kb) {  
            keyboard_int_handler();

            if(readKey){
              scancode_bytes[counter++]=scancode;
              if(scancode!=TWO_BYTES){
                counter = 0;
                Event ev;
                ev.type = KEYBOARD;
                ev.data.scanCode = scancode;
                printf("** EVENT keyboard \n");
                pushEvent(eventQueue, ev);
              }
            }

          }
          break;
        default:
          break; 
			    
      }
    }

    
    sp_write();
    sp_read();

    while(!eventQueueIsEmpty(eventQueue)){
      Event event = popEvent(eventQueue);
      switch (event.type)
      {
      case MOUSE:
          updateCursor(&cursor,& event.data.mouse_packet);
          
          inputHandler(&state,&event);

        break;
      case NEW_FRAME:
        inputHandler(&state,&event);
            
        drawCursor(video_mem_buffer,&cursor);
        copy_from_buffer();
        break;
      case KEYBOARD:
        inputHandler(&state,&event);
      break;
      default:
        break;
      }

    }

  }

  freeEventQueue(eventQueue);

  if(won)
    sp_emptyTransmitQueue(); //make sure that you send the winning move
  
  timer_unsubscribe_int();
  keyboard_unsubscribe_int();
  
  sp_unsubscribe_int();
  
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
