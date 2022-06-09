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
#define COLOR_1 0x0010
#define COLOR_2 0x0037
#define INDEXED_MODE 0x105


int (init_graphics_mode)(uint16_t mode);

int (init_graphics)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate);

int (return_text_mode)();

int (map_vram)(char ** video_mem_pointer, int n, uint16_t mode);

int(vg_draw_mouse)(xpm_image_t img,uint16_t x,uint16_t y);

int(vg_clear_image)(xpm_image_t img,uint16_t x,uint16_t y);

int (create_image)(xpm_map_t xpm, xpm_image_t *image);

unsigned (get_h_res)();

unsigned (get_v_res)();

void (copy_from_buffer)();

void (copy_buffers)(void* vm1, void* vm2);


int (draw_rectangle)(char* board_mem, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (draw_hline)(char* board_mem,uint16_t 	x,uint16_t 	y,uint16_t 	len,uint32_t 	color );

int(draw_image)(char *board_mem, xpm_image_t img,uint16_t x,uint16_t y);

void vg_clear(char * buffer);

void (draw_name)(char * video_mem_buffer, uint8_t* make_code);
