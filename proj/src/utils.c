#include <lcom/lcf.h>

#include <stdint.h>
#include "i8254.h"

#define FAIL 1
#define OK 0

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == 0)
    return FAIL;

  *lsb = val & 0x00FF; 
  return OK;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == 0)
    return FAIL;

  *msb = (val & 0xFF00) >> 8; 
  return OK;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t value32 = 0;
 
  if(sys_inb(port, &value32) == FAIL)
    return FAIL;

  *value = (uint8_t) value32;
  return OK;
} 



