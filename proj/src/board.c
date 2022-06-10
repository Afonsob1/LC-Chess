#include "board.h"
#include "piece.h"
#include "videocard.h"
#include <string.h>

extern char* video_mem_buffer;

unsigned vertical_margin = 128; //so that rectangle_width and rectangle_weight is an integer(80)
float horizontal_margin_percentage = 0.20;
unsigned rectangle_width;
unsigned rectangle_height;
unsigned left_rectangle_width;
unsigned right_rectangle_width;
bool redraw_board =false;

void createPiece(Board* board, PieceType type, int x, int y){
    Piece *p = malloc(sizeof(Piece));
    
    unsigned absolute_x = x*rectangle_width + left_rectangle_width;
    unsigned absolute_y = vertical_margin/2+y*rectangle_height;
    initPiece(p, type, absolute_x, absolute_y);

    board->board[get_position(x, y)] = p;
}

int getScreenX( int col){
    return col*rectangle_width + left_rectangle_width;

}

int getScreenY( int row){
    return  vertical_margin/2+row*rectangle_height;
}

void initBoard(Board* board){
    board->mem_board = NULL;
    map_vram(&board->mem_board, 2, 0x115);

    board->mem_pieces = NULL;
    map_vram(&board->mem_pieces, 3, 0x115);

    /* init variables used for drawing*/
    rectangle_width = (get_v_res() - vertical_margin)/8;
    rectangle_height = (get_v_res() - vertical_margin)/8;
    left_rectangle_width = (get_h_res() - 8*rectangle_width)*horizontal_margin_percentage;
    right_rectangle_width = get_h_res() - 8*rectangle_width - left_rectangle_width;

    memset(board->board, 0, 64*sizeof(Piece*));
    
    /* create pieces*/
    /* Black*/
    
    createPiece(board, b_rook   , 0, 0);
    createPiece(board, b_knight , 1, 0);
    createPiece(board, b_bishop , 2, 0);
    createPiece(board, b_queen  , 3, 0);
    createPiece(board, b_king   , 4, 0);
    createPiece(board, b_bishop , 5, 0);
    createPiece(board, b_knight , 6, 0);
    createPiece(board, b_rook   , 7, 0);
    
    createPiece(board, b_pawn , 0, 1);
    createPiece(board, b_pawn , 1, 1);
    createPiece(board, b_pawn , 2, 1);
    createPiece(board, b_pawn , 3, 1);
    createPiece(board, b_pawn , 4, 1);
    createPiece(board, b_pawn , 5, 1);
    createPiece(board, b_pawn , 6, 1);
    createPiece(board, b_pawn , 7, 1);
    
    /* Whites */
    
    createPiece(board, w_rook   , 0, 7);
    createPiece(board, w_knight , 1, 7);
    createPiece(board, w_bishop , 2, 7);
    createPiece(board, w_queen  , 3, 7);
    createPiece(board, w_king   , 4, 7);
    createPiece(board, w_bishop , 5, 7);
    createPiece(board, w_knight , 6, 7);
    createPiece(board, w_rook   , 7, 7);
    
    createPiece(board, w_pawn , 0, 6);
    createPiece(board, w_pawn , 1, 6);
    createPiece(board, w_pawn , 2, 6);
    createPiece(board, w_pawn , 3, 6);
    createPiece(board, w_pawn , 4, 6);
    createPiece(board, w_pawn , 5, 6);
    createPiece(board, w_pawn , 6, 6);
    createPiece(board, w_pawn , 7, 6);
    

}


void drawBoard(Board* board){

    vg_clear(board->mem_board);
    /*draw_rectangle(board->mem_board, 0,0,left_rectangle_width,get_v_res(),COLOR_BLACK);
    draw_rectangle(board->mem_board, left_rectangle_width + 8*rectangle_width, 0, 
                        right_rectangle_width,get_v_res(),COLOR_BLACK);
    */
    

    gid_t color1,color2, atual_color;
    color1 = 0x3cb043;
    color2 = 0xFFFDD0;

    atual_color = color1; 
    
    for(in_port_t row=0;row<8;row++){
        for(in_port_t col=0;col<8;col++){
            atual_color = (atual_color==color1 ? color2 : color1); 
            draw_rectangle(board->mem_board, col*rectangle_width+left_rectangle_width,row*rectangle_height+vertical_margin/2,
            rectangle_width,rectangle_height,atual_color);
        }
        
        atual_color = (atual_color==color1 ? color2 : color1);
    }

}

void updateBoard(Board* board, bool animation){
    /*percorrer pecas */
    for(in_port_t row=0;row<8;row++){
        for(in_port_t col=0;col<8;col++){
            Piece * p = board->board[get_position(col,row)];
            if(p && p->is_moving){
                if(animation) redraw_board |= updatePiece(p);
                else redraw_board |= updatePiece_no_animation(p);
            }
        }
    }

}


void drawBoardPieces(Board* board){
     if (redraw_board){
        drawBoard(board);
        redraw_board = false;
     }
    copy_buffers(video_mem_buffer, board->mem_board);
   
    /*percorrer pecas e desenhar se n se estiver a mover*/
    for(in_port_t row=0;row<8;row++){
        for(in_port_t col=0;col<8;col++){
            Piece * p = board->board[get_position(col,row)];
            if(p && !p->is_moving)
                drawPieces(video_mem_buffer, p);
        }
    }

    
    /*percorrer pecas e desenhar se se estiver a mover*/
    for(in_port_t row=0;row<8;row++){
        for(in_port_t col=0;col<8;col++){
            Piece * p = board->board[get_position(col,row)];
            if(p && p->is_moving)
                drawPieces(video_mem_buffer, p);
        }
    }
    

}


int getBoardX(int absoluteX){
    if(rectangle_width!=0)
        return (absoluteX - left_rectangle_width)/rectangle_width;
    else
        return -1;
}

int getBoardY(int absoluteY){
    if(rectangle_height!=0)
        return (absoluteY - vertical_margin/2)/rectangle_height;
    else
        return -1;
}


void possiblePosition(Board* board, Piece* piece, int x, int y){
    switch (piece->type)
    {
    case b_pawn:
        if(!piece->pawn_moved) draw_rectangle(board->mem_board,getScreenX(x),getScreenY(y + 2),59,59,0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x),getScreenY(y + 1),59,59,0xed820e);
        break;
    case w_pawn:
        if(!piece->pawn_moved) draw_rectangle(board->mem_board,getScreenX(x),getScreenY(y - 2),59,59,0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x) ,getScreenY(y - 1),59,59, 0xed820e);
        break;
    case w_knight:
    case b_knight:
        draw_rectangle(board->mem_board,getScreenX(x + 2) ,getScreenY(y - 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x + 2) ,getScreenY(y + 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 2) ,getScreenY(y - 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 2) ,getScreenY(y + 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x + 1) ,getScreenY(y + 2),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x + 1) ,getScreenY(y - 2),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 1) ,getScreenY(y - 2),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 1) ,getScreenY(y + 2),59,59, 0xed820e);
        break;
    case w_king:
    case b_king:
        draw_rectangle(board->mem_board,getScreenX(x + 1) ,getScreenY(y + 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x + 1) ,getScreenY(y),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x + 1) ,getScreenY(y - 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x) ,getScreenY(y + 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x) ,getScreenY(y -1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 1) ,getScreenY(y + 1),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 1) ,getScreenY(y),59,59, 0xed820e);
        draw_rectangle(board->mem_board,getScreenX(x - 1) ,getScreenY(y - 1),59,59, 0xed820e);
        break;
    case b_rook:
    case w_rook:
        for (int i = 0; i < 8; i++){
            draw_rectangle(board->mem_board, getScreenX(x), getScreenY(i),59,59, 0xed820e);
        }
        for (int i = 0; i < 8; i++){
            draw_rectangle(board->mem_board, getScreenX(i), getScreenY(y),59,59, 0xed820e);
        }
       break;
    case b_bishop:
    case w_bishop:
        for (int i = 0; i < 8; i++){
            draw_rectangle(board->mem_board,getScreenX(x+i) ,getScreenY(y + i ),59,59, 0xed820e);
            draw_rectangle(board->mem_board,getScreenX(x+i) ,getScreenY(y - i ),59,59, 0xed820e);
            draw_rectangle(board->mem_board,getScreenX(x-i) ,getScreenY(y - i ),59,59, 0xed820e);
            draw_rectangle(board->mem_board,getScreenX(x-i) ,getScreenY(y + i ),59,59, 0xed820e);
        }
        break;
    case w_queen:
    case b_queen:
        for (int i = 0; i < 8; i++){
            draw_rectangle(board->mem_board,getScreenX(x+i) ,getScreenY(y + i ),59,59, 0xed820e);
            draw_rectangle(board->mem_board,getScreenX(x+i) ,getScreenY(y - i ),59,59, 0xed820e);
            draw_rectangle(board->mem_board,getScreenX(x-i) ,getScreenY(y - i ),59,59, 0xed820e);
            draw_rectangle(board->mem_board,getScreenX(x-i) ,getScreenY(y + i ),59,59, 0xed820e);
        }
        for (int i = 0; i < 8; i++){
            draw_rectangle(board->mem_board, getScreenX(x), getScreenY(i),59,59, 0xed820e);
        }
        for (int i = 0; i < 8; i++){
            draw_rectangle(board->mem_board, getScreenX(i), getScreenY(y),59,59, 0xed820e);
        }
        break;
    default:
        break;
    }
    copy_buffers(video_mem_buffer, board->mem_board);
}
