#include <lcom/lcf.h>
#include <utils.h>

#define MOUSE_IRQ 12
#define OUT_BUFF 0x60
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64
#define BYTE_TO_MOUSE 0xD4
#define KBC_ST_IBF 0x60
#define EN_DATA_REPORTING 0xF4
#define ACK_BYTE_OK 0xFA
#define DIS_DATA_REPORTING 0xF5
#define OBF_FULL BIT(0)
#define AUX BIT(5)
#define TIMEOUT_ERR BIT(6)
#define PAR_ERR BIT(7)
#define RB_PRESSED BIT(1)
#define MB_PRESSED BIT(2)
#define LB_PRESSED BIT(0)
#define X_OVFL BIT(6)
#define Y_OVFL BIT(7)
#define X_SIGN BIT(4)
#define Y_SIGN BIT(5)
#define DELAY_US 20000
#define TIMER0_IRQ 0
#define FIRST_PACKET_BYTE BIT(3)

enum machine_state{
  OFF,
  GOING_UP,
  GOING_DOWN,
  DONE
};

int (mouse_subscribe_int)(uint8_t* bit_no);

int (mouse_unsubscribe_int)();

int (timer_subscribe_int)(uint8_t* bit_no);

int (timer_unsubscribe_int)();

int (mouse_disable_data_reporting)();

int (issue_kbc_command)(uint8_t command,uint8_t *arg);

int (read_kbc_return)(uint8_t *data);

void (timer_ih)();
