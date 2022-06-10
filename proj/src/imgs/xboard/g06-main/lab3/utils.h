#include <lcom/lcf.h>

#include <stdint.h>

#define LSB_MASK 0x00FF
#define MSB_MASK 0xFF00

int(util_get_LSB)(uint16_t val, uint8_t *lsb);

int(util_get_MSB)(uint16_t val, uint8_t *msb);

int (util_sys_inb)(int port, uint8_t *value);
