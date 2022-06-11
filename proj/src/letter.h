#pragma once


#include <lcom/lcf.h>

#define BACK_SPACE 0x8e
#define ENTER 0x9c

#define LETTER_A    0x1e
#define LETTER_B    0X30
#define LETTER_C    0x2e
#define LETTER_D    0x20
#define LETTER_E    0x12
#define LETTER_F    0x21
#define LETTER_G    0x22
#define LETTER_H    0x23
#define LETTER_I    0x17
#define LETTER_J    0x24
#define LETTER_K    0x25
#define LETTER_L    0x26
#define LETTER_M    0x32
#define LETTER_N    0x31
#define LETTER_O    0x18
#define LETTER_P    0x19
#define LETTER_Q    0x10
#define LETTER_R    0x13
#define LETTER_S    0x1f
#define LETTER_T    0x14
#define LETTER_U    0x16
#define LETTER_V    0x2f
#define LETTER_W    0x11
#define LETTER_X    0x2d
#define LETTER_Y    0x15
#define LETTER_Z    0x2c


/*
* Create all images for letters and store them in letters array
*/
void initLetter();

/**
 * @brief draws the name of the player into the buffer on the specified position with the specified spacing
 * @param video_mem_buffer the buffer we will draw the letters into
 * @param make_code pointer to the beginning of the array of break codes
 * @param base_x base horizontal position
 * @param base_y base vertical position
 * @param spacing spacing between each letter 
 */
void (draw_name)(char * video_mem_buffer, uint8_t* make_code, int base_x, int base_y, int spacing);


/**
 * @brief return the xpm image depending on the breakcode passed in the argument. 
 * 
 * @param byte breakcode of the character we want to draw.
 */
xpm_image_t* getLetter(uint8_t byte);
