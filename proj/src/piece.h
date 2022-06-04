#pragma once

#include <lcom/lcf.h>
#include "videocard.h"

typedef enum PieceType{
    b_pawn,
    b_knight,
    b_bishop,
    b_rook,
    b_queen,
    b_king,
    w_pawn,
    w_knight,
    w_bishop,
    w_rook,
    w_queen,
    w_king,
} PieceType;


typedef struct Piece
{
    uint16_t absolute_x;    // screen x 
    uint16_t absolute_y;    // screen x
    xpm_image_t image;
    PieceType type;
    bool is_moving;
    bool pressed;
} Piece;

void initPiece(Piece* piece, PieceType piece_type,  int absolute_x, int absolute_y);

void drawPieces(char* mem, Piece* piece);

void movePiece(Piece* piece,int x, int y);


