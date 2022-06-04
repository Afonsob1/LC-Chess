#pragma once
#include <lcom/lcf.h>
#include "videocard.h"
#include "board.h"
#include "imgs/xboard/cursor.h"
#include <stdbool.h>


typedef struct{
    uint16_t x;
    uint16_t y;
    bool pressed;
    xpm_image_t image;
    Piece* pressed_piece;
}Cursor;


void initCursor(Cursor* cursor);

void updateCursor(Board* board,Cursor* cursor, struct packet* pp);

void drawCursor(char* mem,Cursor* cursor);

