#ifndef BOARD_H
#define BOARD_H


#include "piece.h"
#include "videocard.h"
#include <lcom/lcf.h>

#define get_position(x,y) (x + y*8)

typedef struct
{
    char * mem_board;
    char * mem_pieces;
    Piece* board[64];
}Board;

/*
    Board Constructor 
*/
void initBoard(Board* board);

void drawBoard(Board* board);

void drawBoardPieces(Board* board);
#endif

int getScreenX( int col);

int getScreenY( int row);

int getBoardX(int absoluteX);

int getBoardY(int absoluteY);

void updateBoard(Board* board, bool animation);


