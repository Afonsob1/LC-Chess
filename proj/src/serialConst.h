#ifndef _SERIAL_PORT_CONSTANTS_H_
#define _SERIAL_PORT_CONSTANTS_H_

#include <lcom/lcf.h>

#define VM_CONNECTED 0xFA
#define RECEIVE_VM_CONNECTED 0xFD
#define SP_COM 0x3F8

#define SP_IRQ 4

#define SP_RBR 0
#define SP_THR 0
#define SP_DLL 0
#define SP_DLM 1
#define SP_IER 1
#define SP_IIR 2
#define SP_FCR 2
#define SP_LCR 3
#define SP_LSR 5

//LCR
#define SP_8BITS            (BIT(0) & BIT(1))
#define SP_DLAB             (BIT(7))

#define SP_RECEIVER                     BIT(0)
#define SP_TRANSMITTER_HOLDING_EMPTY        BIT(5)

#define SP_RECEIVED_INTERRUPT               BIT(0)
#define SP_TRANSMITTER_INTERRUPT            BIT(1)
#define SP_RECEIVER_LINE_INTERRUPT          BIT(2)

#define SP_NO_PENDING_INTERRUPT             BIT(0)
#define SP_RECEIVER_LINE                    (BIT(1) & BIT(2) & ~BIT(3))

#define SP_CHAR_TIMEOUT                     (~BIT(1) & BIT(2) & BIT(3))
#define SP_RECEIVED_DATA                    (~BIT(1) & BIT(2) & ~BIT(3))
#define SP_TRANSMITTER_HOLDING              (BIT(1) & ~BIT(2) & ~BIT(3))
#define SP_MODEM_STATUS                     (~BIT(1) & ~BIT(2) & ~BIT(3))

#define SP_ENABLE_FIFO                      BIT(0)
#define SP_CLEAR_RECEIVER_FIFO              BIT(1)
#define SP_CLEAR_TRASMITTER_FIFO            BIT(2)

#endif
