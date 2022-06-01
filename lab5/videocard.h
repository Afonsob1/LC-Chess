#include <lcom/lcf.h>


#define LINEAR_FRAMEBUFFER BIT(14)
#define VIDEO_CARD_INT 0x10
#define STATUS_REG 0x64
#define OUT_BUF 0x60
#define OUT_BUF_FULL BIT(0)
#define AUX BIT(5)
#define TIMER0_IRQ 0
#define TIMEOUT_ERR BIT(6)
#define PARITY_ERR BIT(7)
#define TWO_BYTES 0xE0
#define LSB_MASK 0x00FF
#define COLOR_BLACK 0x0000
#define INDEXED_MODE 0x105


int (set_vbe_mode)(uint16_t mode);

int (map_vram)(uint16_t mode);

int (timer_subscribe_int)(uint8_t * bit_no);

int (timer_unsubscribe_int)();

void (timer_int_handler)();

int(kb_subscribe_int)(uint8_t *bit_no);

int(kb_unsubscribe_int)();

int(util_sys_inb)(int port, uint8_t *value);

int(kb_int_handler)();

int(map_vram)(uint16_t mode);

int (vg_draw_hline)(uint16_t x,uint16_t y, uint16_t len,uint32_t color);

int (vg_draw_rectangle)(uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color);


<<<<<<< HEAD
int (vg_draw_matrix)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
=======
int(draw_image)(xpm_image_t img,uint16_t x,uint16_t y);
>>>>>>> origin/animation

int(vg_draw_image)(uint16_t x, uint16_t y, xpm_image_t img);

int (vg_clear_image)(uint16_t x, uint16_t y, xpm_image_t img);
