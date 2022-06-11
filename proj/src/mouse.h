#include <lcom/lcf.h>
#include "utils.h"
#include "cursor.h"

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

/**
 * @brief subscribes the interruption line for the keyboard interruptions
 * @param bit_no number of bit we want to set as one.
 */
int (mouse_subscribe_int)(uint8_t* bit_no);
/**
 * @brief unsubscribes the interruption line for the mouse interruptions
 * 
 */
int (mouse_unsubscribe_int)();
/**
 * @brief this functions deals with the mouse interruptions. reads every packet and updates the pp struct
 * 
 */
void (mouse_ih)();

/**
 * @brief writes the kbc command on the kbc 0x64 register
 * @param command command we want to write to the kbc
 */
int (write_kbc_command)(uint8_t command);
/**
 * @brief reads kbc from the 0x60 buffer and then passes it by reference.
 * @param ret variable passes by reference where the return byte will be placed
 */
int (read_kbc_return)(uint8_t* ret);
/**
 * @brief writes the argument arg on the kbc register 0x60
 * @param arg argument to be written
 */
int (write_kbc_arg)(uint8_t arg);
/**
 * @brief disables the data reporting on the mouse by sending specific commands to the kbc
 * 
 */
int (mouse_disable_data_reporting)();

