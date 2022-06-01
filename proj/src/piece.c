#include "piece.h"
#include <lcom/lcf.h>
#include "videocard.h"
#include "imgs/xboard/pdl80.h"
#include "imgs/xboard/pll80.h"
#include "imgs/xboard/rdl80.h"
#include "imgs/xboard/rll80.h"
#include "imgs/xboard/qll80.h"
#include "imgs/xboard/qdl80.h"
#include "imgs/xboard/nll80.h"
#include "imgs/xboard/ndl80.h"
#include "imgs/xboard/bll80.h"
#include "imgs/xboard/bdl80.h"
#include "imgs/xboard/kll80.h"
#include "imgs/xboard/kdl80.h"

extern char* video_mem_buffer;
#define VELOCITY 1

void initPiece(Piece* piece, PieceType piece_type,  int absolute_x, int absolute_y){
    switch (piece_type){
        case b_pawn:
            create_image(pdl80, &(piece->image));
            break;

        case b_knight:
            create_image(ndl80, &(piece->image));
            break;
            
        case b_bishop:
            create_image(bdl80, &(piece->image)); 
            break;
            
        case b_rook:
            create_image(rdl80, &(piece->image));
            break;
            
        case b_queen:
            create_image(qdl80, &(piece->image));
            break;
            
        case b_king:
            create_image(kdl80, &(piece->image));
            break;
            
        case w_pawn:
            create_image(pll80, &(piece->image));
            break;

        case w_knight:
            create_image(nll80, &(piece->image));
            break;
            
        case w_bishop:
            create_image(bll80, &(piece->image));
            break;
            
        case w_rook:
            create_image(rll80, &(piece->image));
            break;
            
        case w_queen:
            create_image(qll80, &(piece->image));
            break;
            
        case w_king:
            create_image(kll80, &(piece->image));
            break;
        
        default:
            panic("CRIAR PEÇA");
            return;
    }

    

    piece->absolute_x = absolute_x;
    piece->absolute_y = absolute_y;
    piece->type = piece_type;
    piece->is_moving = false;
}


void drawPieces(char* mem, Piece* piece){
    if(piece == NULL) return;
    draw_image(mem, piece->image, piece->absolute_x, piece->absolute_y);
}


void updatePiece(Piece* piece){
    
    if(piece->absolute_x == piece->target_x && piece->absolute_y == piece->target_y){
        piece->is_moving = false;
        return;
    }

    int addX = piece->target_x - piece->absolute_x; 
    if(addX > 0){
        piece->absolute_x++;
    }
    else if (addX < 0){
        piece->absolute_x--;
    }

    int addY = piece->target_y - piece->absolute_y; 
    if(addY > 0){
        piece->absolute_y++;
    }else if (addY < 0){
        piece->absolute_y--;
    }

}


void movePiece(Piece* piece,int x, int y){
    piece->is_moving = true;
    
    piece->target_x = x;
    piece->target_y = y;
}


