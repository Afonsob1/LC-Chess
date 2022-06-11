#pragma once
#include <lcom/lcf.h>
#include "board.h"
#include <stdbool.h>


typedef struct{
    uint16_t x;
    uint16_t y;
    bool pressed;
    bool released;
    xpm_image_t image;
}Cursor;


void initCursor(Cursor* cursor);

void updateCursor(Cursor* cursor, struct packet* pp);

void drawCursor(char* mem,Cursor* cursor);

