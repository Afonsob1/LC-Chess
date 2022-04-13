#include <lcom/lcf.h>

#include <stdint.h>

#define LSB_MASK 0x00FF
#define MSB_MASK 0xFF00

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  *lsb = (uint8_t) val&LSB_MASK;
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  *msb = (uint8_t) ((val&MSB_MASK) >> 8);
  return 0;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t aux;
  int r;
  if((r=sys_inb(port,&aux))!=0)
    return r;
  *value = (uint8_t) aux&LSB_MASK;
  return 0;   
}

