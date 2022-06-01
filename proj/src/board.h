#define pragma once

#include "piece.h"
#include "videocard.h"
#include <lcom/lcf.h>

#define get_position(x,y) (x + y*8)

typedef struct Board
{
    char * mem_board;
    char * mem_pieces;
    Piece* board[64];
} Board;

/*
    Board Constructor 
*/
void initBoard(Board* board);

void drawBoard(Board* board);

void drawBoardPieces(Board* board);


int getScreenX( int col);

int getScreenY( int row);


void updateBoard(Board* board);


