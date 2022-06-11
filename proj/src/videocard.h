#include <lcom/lcf.h>

#define STATUS_REG 0x64
#define OUT_BUF 0x60
#define OUT_BUF_FULL BIT(0)
#define AUX BIT(5)
#define TIMER0_IRQ 0
#define TIMEOUT_ERR BIT(6)
#define PARITY_ERR BIT(7)
#define LSB_MASK 0x00FF
#define COLOR_BLACK 0x0000
#define COLOR_DARK_BLUE 0x0001
#define COLOR_GREEN 0x0002
#define COLOR_CYAN 0x0003
#define COLOR_1 0x3cb043
#define COLOR_2 0xFFFDD0
#define INDEXED_MODE 0x105

/**
 * @brief initializes the videocard with the mode given and maps the vram into a physical adress
 * @param mode that we want to initialize
 */
int (init_graphics_mode)(uint16_t mode);


//int (init_graphics)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

/**
 * @brief sets the videocard mode to text mode
 * 
 */
int (return_text_mode)();

/**
 * @brief maps the vram into a physical adress
 * @param video_mem_pointer physical adress where we want to map the vram
 * @n number of buffer where the vram is initialize
 * @mode mode that we want to use
 */
int (map_vram)(char ** video_mem_pointer, int n, uint16_t mode);

/**
 * @brief draw the mouse image int he screen
 * @param img image of the mouse 
 * @param x x where we want to draw the mouse
 * @param y y where we want to draw the mouse
 */
int(vg_draw_mouse)(xpm_image_t img,uint16_t x,uint16_t y);

//int(vg_clear_image)(xpm_image_t img,uint16_t x,uint16_t y);

/**
 * @brief creates a xpm_imate_t with a xpm_map_t
 * @param xpm xpm that we want to convert into an image
 * @param image pointer to where we want to save the image
 */
int (create_image)(xpm_map_t xpm, xpm_image_t *image);

/**
 * @brief get the horizontal resolution
 * @return vertical resolution
 */
unsigned (get_h_res)();

/**
 * @brief get the vertical resolution
 * @return horizontal resolution
 */
unsigned (get_v_res)();

/**
 * @brief copies to the video_mem the video_mem_buffer
 *
 */
void (copy_from_buffer)();
/**
 * @brief copies one buffer to other buffer
 * @param vm1 buffer where we want to copy the other buffer to
 * @param vm2 buffer that we want to copy
 */
void (copy_buffers)(void* vm1, void* vm2);

/**
 * @brief draws an rectangle into the buffer in the argument in the position x and y with width , height and color
 * @param board_mem buffer where we draw
 * @param xpm_image_t xpm_img we want to draw
 * @param x horizontal position
 * @param y vertical position
 * @param width width of the rectangle
 * @param height height of the rectan */

 int (draw_rectangle)(char* board_mem, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
/**
 * @brief draws an horizontal line into the buffer in the argument in the position x and y with len and color
 * @param board_mem buffer where we draw
 * @param xpm_image_t xpm_img we want to draw
 * @param x horizontal position
 * @param len lenght of the line
 * @param color color of the line in this project we use direct color mode.
 */
int (draw_hline)(char* board_mem,uint16_t 	x,uint16_t 	y,uint16_t 	len,uint32_t 	color );
/**
 * @brief draws a xpm image (img) into the buffer in the argument in the position x and y
 * @param board_mem buffer where we draw
 * @param xpm_image_t xpm_img we want to draw
 * @param x horizontal position
 * @param y vertical position
 */
int(draw_image)(char *board_mem, xpm_image_t img,uint16_t x,uint16_t y);
/**
 * @brief sets all the memory starting at the buffer pointer to contain the value 0 (black screen)
 * @param buffer video memory buffer we want to set to 0
 */
void vg_clear(char * buffer);

