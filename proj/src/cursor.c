#include "cursor.h"
#include "videocard.h"
#include "imgs/xboard/cursor.h"


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


void updateCursor(Cursor* cursor, struct packet* pp){
    int32_t x = pp->delta_x;
    int32_t y = pp->delta_y;
    cursor->released=false;
    
    /*move mouse*/
    
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

    /* clicks */
    if (pp->lb && !cursor->pressed){//clicked something
        printf(" ****** PRESSED\n");
        cursor->pressed=true;
    }else if(!pp->lb && cursor->pressed){
        printf(" ****** RELEASED\n");
        cursor->pressed=false;
        cursor->released=true;
    }

}
