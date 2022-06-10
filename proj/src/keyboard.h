#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>

#define STAT_REG 0x64
#define OUT_BUF 0x60
#define TWO_BYTES 0xe0
#define OUT_BUF_EMPTY -1
#define KB_IRQ 0x01
#define BREAK_CODE BIT(7)
#define OBF_FULL BIT(0)
#define PAR_ERR BIT(7)
#define TIMEOUT_ERR BIT(6)
#define AUX BIT(5)
#define TIMER0_IRQ 0
#define KBC_ST_IBF BIT(1)
#define DELAY_US 20000
#define KBC_CMD_REG 0x64
#define KBC_IBF 0x60
#define READ_KBC_CMD 0x20
#define WRITE_KBC_CMD 0x60
#define CHECK_KBC 0xAA
#define CHECK_KBD_INTERFACE 0xAB
#define DISABLE_KBD_INTERFACE 0xAD
#define ENABLE_KBD_INTERFACE 0xAE
#define KBC_OK 0x55
#define KBC_ERROR 0xFC

int (keyboard_subscribe_int)(uint8_t * bit_no);

int (keyboard_unsubscribe_int)();

int (keyboard_int_handler)();

int (read_command_byte)(uint8_t* command_byte);

int (write_command_byte)(uint8_t command_byte);

void drawLetter(uint8_t byte);
