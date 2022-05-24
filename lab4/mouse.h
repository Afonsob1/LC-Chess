#include <lcom/lcf.h>
#include "utils.h"

#define MOUSE_IRQ_LINE 12
#define TIMER0_IRQ 0
#define IN_BUF 0x60
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define CTRL_REG 0x64
#define OBF_FULL BIT(0)
#define IBF_FULL BIT(1)
#define AUX BIT(5)
#define TIMEOUT_ERR BIT(6)
#define PAR_ERR BIT(7)
#define FIRST_BYTE BIT(3)
#define LEFT_BUTTON BIT(0)
#define MIDDLE_BUTTON BIT(2)
#define RIGHT_BUTTON BIT(1)
#define X_SIGN BIT(4)
#define Y_SIGN BIT(5)
#define X_OVFL BIT(6)
#define Y_OVFL BIT(7)
#define SIGN_EXTENSION 0xFF00
#define BYTE_TO_MOUSE 0xD4
#define DIS_DATA_REPORTING 0xF5

int (mouse_subscribe_int)(uint8_t* bit_no);

int (mouse_unsubscribe_int)();

void (mouse_ih)();

int (mouse_disable_data_reporting)();

int (write_kbc_command)(uint8_t command);

int (read_kbc_return)(uint8_t* ret);

int (write_kbc_arg)(uint8_t arg);

int (mouse_disable_data_reporting)();

int (timer_subscribe_int)(uint8_t* bit_no);

int (timer_unsubscribe_int)();

void (timer_int_handler)();
