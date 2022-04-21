#include <lcom/lcf.h>

#define STATUS_REG 0x64
#define OUT_BUF 0x60

#define OUT_BUF_FULL BIT(0)
#define AUX BIT(5)
#define TIMEOUT_ERR BIT(6)
#define PARITY_ERR BIT(7)
#define TWO_BYTES 0xE0
#define LSB_MASK 0x00FF
#define COLOR_BLACK 0x0000


int (init_graphics_mode)(uint16_t mode);

int (return_text_mode)();

int (map_vram)(uint16_t mode);

int(kb_subscribe_int)(uint8_t *bit_no);

int(kb_unsubscribe_int)();

int(kb_int_handler)();

int(vg_draw_rectangles)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);
