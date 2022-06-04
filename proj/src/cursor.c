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
        movePiece(cursor->pressed_piece,cursor->x,cursor->y);
    }
    
    if (pp->lb && !cursor->pressed){
        cursor->pressed = true;
        unsigned board_col = (cursor->x - left_rectangle_width)/rectangle_width;
        unsigned board_row = (cursor->y - vertical_margin/2)/rectangle_height;
        if(board->board[board_row*8+board_col]!=NULL){
            cursor->pressed_piece= board->board[board_row*8+board_col];
        }
    }
    else if(!pp->lb && cursor->pressed){
     cursor->pressed = false;
     cursor->pressed_piece=NULL;
    }
}
