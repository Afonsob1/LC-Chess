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

/**
 * @brief initializes ther cursor in the position x = 0 e y = 0 with image arrow
 * 
 * @param cursor cursor that we want to initialize 
 */
void initCursor(Cursor* cursor);

/**
 * @brief updates cursor with the mouse packet given by the mouse interrupt handler
 * 
 * @param cursor cursor that we want to update
 * @param pp packet given by mouse interrupt handler
 */
void updateCursor(Cursor* cursor, struct packet* pp);

/**
 * @brief draws in cursor in a buffer
 * @param mem  buffer that we want to draw the cursor to
 * @param cursor cursor that we want to draw
*/
void drawCursor(char* mem,Cursor* cursor);

