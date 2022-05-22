#define pragma once

#include "piece.h"
#include <lcom/lcf.h>

#define get_position(x,y) (x + y*8)

typedef struct Board
{
    Piece* board[64];
} Board;

/*
    Board Constructor 
*/
void initBoard(Board* board);

void drawBoard(Board* board);


