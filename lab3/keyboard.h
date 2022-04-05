#include <lcom/utils.h>
#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>

extern uint8_t c;
extern int counter;
#define DELAY_US    20000
#define OUTPUT_BUFFER 0x60
#define STAT_REG 0x64
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define DELAY_US    20000
#define KBC_ST_IBF 0x02
#define KBC_OUT_BUF 0x01
#define KBC_PAR_ERROR 0x80
#define KBC_TO_ERROR 0x40
#define KBC_INPUT_BUFFER 0x60
#define ReadCMDByte 0x20
#define WriteCMDByte 0x60
#define checkKBC 0xAA
#define CheckKBInterface 0xAB
#define DisableKBDInterface 0xAD
#define EnableKBDInterface 0xAE
#define KBCTestOK 0x55
#define KBCTestFailed 0xFC
#define OUTPUT_BUFFER 0x60
#define STAT_REG 0x64

void (kbc_ih)();

int (issue_command)(uint8_t* command,uint8_t args);

int (read_kbc)(uint8_t *);

int (write_kbc)(uint8_t);

int (write_kbc_args)(uint8_t,uint8_t);

void (timer_int_handler)();
