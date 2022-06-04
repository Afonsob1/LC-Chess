#include "cursor.h"
#include "videocard.h"
#include "board.h"
#include "imgs/xboard/cursor.h"

extern unsigned vertical_margin; //so that rectangle_width and rectangle_weight is an integer(80)
extern float horizontal_margin_percentage;
extern unsigned rectangle_width;
extern unsigned rectangle_height;
extern unsigned left_rectangle_width;
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
    
    if(cursor->pressed_piece==NULL){
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
        else if(cursor->x+x+80>=(int)get_h_res())
            cursor->x=get_h_res()-81;
        else
            cursor->x+=x;

        if(cursor->y < y)
            cursor->y=0;
        else if(cursor->y-y+80>=(int)get_v_res())
            cursor->y=get_v_res()-81;
        else
            cursor->y-=y;
        setPiecePosition(cursor->pressed_piece,cursor->x-40,cursor->y-40);
    }
    
    unsigned board_col = (cursor->x - left_rectangle_width)/rectangle_width;
    unsigned board_row = (cursor->y - vertical_margin/2)/rectangle_height;

    if (pp->lb && !cursor->pressed){
        cursor->pressed = true;
        if(board->board[board_row*8+board_col]!=NULL){
            cursor->pressed_piece= board->board[board_row*8+board_col];
        }
        cursor->initial_col = board_col;
        cursor->initial_row = board_row;
    }
    else if(!pp->lb && cursor->pressed){
        if(board->board[board_row*8+board_col]!=NULL){
            bool sameTeam = (cursor->pressed_piece->type>5 && board->board[board_row*8+board_col]->type>5) || (cursor->pressed_piece->type<=5 && board->board[board_row*8+board_col]->type<=5);
            if(!sameTeam){
                board->board[cursor->initial_row*8+cursor->initial_col]=NULL;
                board->board[board_row*8+board_col]=cursor->pressed_piece;
            }
            else{
                unsigned absolute_x = cursor->initial_col*rectangle_width + left_rectangle_width;
                unsigned absolute_y = vertical_margin/2+cursor->initial_row*rectangle_height;
                setPiecePosition(cursor->pressed_piece,absolute_x,absolute_y);
            }
        }
        else{
            board->board[cursor->initial_row*8+cursor->initial_col]=NULL;
            board->board[board_row*8+board_col]=cursor->pressed_piece;
        }
        cursor->pressed = false;
        cursor->pressed_piece=NULL;

    }
}
