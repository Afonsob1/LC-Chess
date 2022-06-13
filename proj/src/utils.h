#pragma once
#include <lcom/lcf.h>
#include <stdint.h>

#define LSB_MASK 0x00FF
#define MSB_MASK 0xFF00

/**
 * @brief calcule the least significant byte
 * 
 * @param val   number to get the lsb
 * @param lsb   where to store de least significant byte
 * @return 0 if valid
 */
int(util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief calcule the most significant byte
 * 
 * @param val   number to get the msb
 * @param lsb   where to store de most significant byte
 * @return 0 if valid
 */
int(util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief read a byte from port
 * 
 * @param port   port of the register
 * @param value   where to store the byte read
 * @return 0 if valid
 */
int(util_sys_inb)(int port, uint8_t *value);

