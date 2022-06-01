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

void createPiece(Board* board, PieceType type, int x, int y){
    Piece *p = malloc(sizeof(Piece));
    
    unsigned absolute_x = x*rectangle_width + left_rectangle_width;
    unsigned absolute_y = vertical_margin/2+y*rectangle_height;
    initPiece(p, type, absolute_x, absolute_y);

    board->board[get_position(x, y)] = p;
}

void initBoard(Board* board){
    board->mem_board = NULL;
    map_vram(&board->mem_board, 2, INDEXED_MODE);

    board->mem_pieces = NULL;
    map_vram(&board->mem_pieces, 3, INDEXED_MODE);

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
    draw_rectangle(board->mem_board, 0,0,left_rectangle_width,get_v_res(),COLOR_BLACK);
    draw_rectangle(board->mem_board, left_rectangle_width + 8*rectangle_width, 0, 
                        right_rectangle_width,get_v_res(),COLOR_BLACK);

    

    gid_t color1,color2, atual_color;
    color1 = COLOR_1;
    color2 = COLOR_2;

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

void drawBoardPieces(Board* board){
    
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



