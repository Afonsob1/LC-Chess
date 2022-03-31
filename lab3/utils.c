#include <lcom/lcf.h>
#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  /* To be implemented by the students */
  *lsb = (uint8_t)(val&0x00FF);
  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  *msb = (uint8_t)((val&0xFF00)>>8);
  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
  /* To be implemented by the students */

  uint32_t aux;
  sys_inb(port,&aux);
  *value = aux;

  return 1;
}
