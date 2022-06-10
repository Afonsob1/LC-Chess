#include "cursor.h"
#include "videocard.h"
#include "board.h"
#include "imgs/xboard/cursor.h"

extern unsigned vertical_margin; //so that rectangle_width and rectangle_weight is an integer(80)
extern float horizontal_margin_percentage;
extern unsigned rectangle_width;
extern unsigned rectangle_height;
extern unsigned left_rectangle_width;
int player_number;
//unsigned right_rectangle_width = get_h_res() - 8*rectangle_width - left_rectangle_width;

void initCursor(Cursor* cursor){
    cursor->x=0;
    cursor->y=0;
    create_image(arrow, &cursor->image);
}


void drawCursor(char* mem,Cursor* cursor){
    if(cursor == NULL)
         return;
    draw_image(mem, cursor->image, cursor->x, cursor->y);
}


void updateCursor(Board* board,Cursor* cursor, struct packet* pp){
    int32_t x = pp->delta_x;
    int32_t y = pp->delta_y;
        /*move mouse*/
    
    if(!cursor->cursorMovingPiece){
        if(cursor->x < -x)
            cursor->x=0;
        else if(cursor->x+x+11>=(int)get_h_res())
            cursor->x=get_h_res()-12;
        else
            cursor->x+=x;

        if(cursor->y < y)
            cursor->y=0;
        else if(cursor->y-y+19>=(int)get_v_res())
            cursor->y=get_v_res()-20;
        else
            cursor->y-=y;
    }
    
    else{
        if(cursor->x < -x)
            cursor->x=0;
        else if(cursor->x+x+58>=(int)get_h_res())
            cursor->x=get_h_res()-59;
        else
            cursor->x+=x;

        if(cursor->y < y)
            cursor->y=0;
        else if(cursor->y-y+58>=(int)get_v_res())
            cursor->y=get_v_res()-59;
        else
            cursor->y-=y;
        
            setPiecePosition(cursor->pressed_piece,cursor->x-29,cursor->y-29); //-29 so cursor is in the center of the image
    }
    
    

    if (pp->lb && !cursor->pressed){//clicked something
        cursor->pressed=true;
        printf("Clicked mouse on position %d:%d\n",cursor->x,cursor->y);
        if(getBoardY(cursor->y)>=0 && getBoardY(cursor->y)<8
        &&getBoardX(cursor->x)>=0 && getBoardX(cursor->x)<8){
            
            
            if(board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)]!=NULL){ //clicked piece
                cursor->cursorMovingPiece = true;
                cursor->pressed_piece= board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)];
                cursor->initial_col = getBoardX(cursor->x);
                cursor->initial_row = getBoardY(cursor->y);
            }
            
        }
        
        
    }
    else if(!pp->lb && cursor->pressed){
        cursor->pressed=false;
        
        if(cursor->cursorMovingPiece){
            if(board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)]!=NULL){
                bool sameTeam = (cursor->pressed_piece->type>5 && board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)]->type>5) || (cursor->pressed_piece->type<=5 && board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)]->type<=5);
                if(!sameTeam){
                    setPiecePosition(cursor->pressed_piece,getScreenX(cursor->initial_col),getScreenY(cursor->initial_row));
                    movePiece(cursor->pressed_piece,getScreenX(getBoardX(cursor->x)),getScreenY(getBoardY(cursor->y)));
                    board->board[cursor->initial_row*8+cursor->initial_col]=NULL;
                    board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)]=cursor->pressed_piece;
                }
                else{
                    setPiecePosition(cursor->pressed_piece,getScreenX(cursor->initial_col),getScreenY(cursor->initial_row));
                }
            }
            else{
                setPiecePosition(cursor->pressed_piece,getScreenX(cursor->initial_col),getScreenY(cursor->initial_row));
                movePiece(cursor->pressed_piece,getScreenX(getBoardX(cursor->x)),getScreenY(getBoardY(cursor->y)));
                board->board[cursor->initial_row*8+cursor->initial_col]=NULL;
                board->board[getBoardY(cursor->y)*8+getBoardX(cursor->x)]=cursor->pressed_piece;
            }
            cursor->cursorMovingPiece=false;
            cursor->pressed_piece=NULL;
        }
        
    }
}

bool cursorClickPlayer(Cursor *cursor){
    //player 2
    if((cursor->x > 307 && cursor->x < 466  && cursor->y > 62 && cursor->y <104) && cursor->pressed){
        player_number = 2;
        return true;
    }
    //player 1
    if ((cursor->x > 307 && cursor->x < 466 && cursor->y > 517 && cursor->y <558) &&  cursor->pressed){
        player_number = 1;
        return true;
    }
    return false;
}
