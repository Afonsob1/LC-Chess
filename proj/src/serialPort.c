#include "serialPort.h"
#include "utils.h"

int hook_id_sp;

static Queue * transmitQueue;
static Queue * receiveQueue;



int sp_set_number_of_bits_per_char(uint8_t number_of_bits) {
  uint32_t lcr;

	if (sys_inb(SP_COM + SP_LCR, &lcr) != 0) {return 1;}

  lcr = (lcr & 0xFC) | number_of_bits;

  if (sys_outb(SP_COM + SP_LCR, lcr) != 0) {return 1;}

  return 0;
}

void sp_clear(){

  while(sp_check_read()){
    sp_read();
    tickdelay(micros_to_ticks(30000));
  }

  while(!queueIsEmpty(receiveQueue)){
    removeFromQueue(receiveQueue);
  }
}



int sp_set_bitrate(int bitrate) {
  uint16_t actualRate = 115200 / bitrate;

  uint8_t actualRatemsb = 0, actualRatelsb = 0;
  util_get_LSB(actualRate, &actualRatelsb);
  util_get_MSB(actualRate, &actualRatemsb);

  //SET DLAB
  uint32_t lcr;
	if (sys_inb(SP_COM + SP_LCR, &lcr) != 0) {return -1;}
  lcr = lcr | SP_DLAB;
  if (sys_outb(SP_COM + SP_LCR, lcr) != 0) {return -1;}

  //SET BITRATE
  if (sys_outb(SP_COM + SP_DLL, actualRatelsb) != 0) {return -1;}
  if (sys_outb(SP_COM + SP_DLM, actualRatemsb) != 0) {return -1;}

  //RESET DLAB
  if (sys_inb(SP_COM + SP_LCR, &lcr) != 0) {return -1;}
  lcr = lcr & ~SP_DLAB;
  if (sys_outb(SP_COM + SP_LCR, lcr) != 0) {return -1;}

  return 0;
}

int sp_enable(uint8_t port,uint8_t bit, bool enable) {
   uint32_t aux;

	if (sys_inb(SP_COM + port, &aux) != 0) {return -1;}

  if (enable)
    aux = aux | bit;
  else 
    aux = aux & ~bit;

  if (sys_outb(SP_COM + port, aux) != 0) {return -1;}

  return 0;
}


int sp_configure_init() {
  if (sp_set_number_of_bits_per_char(SP_8BITS) != 0) {return -1;}

  if (sp_set_bitrate(115200) != 0) {return 1;}

  if (sp_enable(SP_IER, SP_RECEIVED_INTERRUPT, true) != 0) {return -1;}
  if (sp_enable(SP_IER, SP_TRANSMITTER_INTERRUPT, true) != 0) {return -1;}
  if (sp_enable(SP_IER, SP_RECEIVER_LINE_INTERRUPT, true) != 0) {return -1;}


  if (sp_enable(SP_FCR, SP_ENABLE_FIFO, true) != 0) {return -1;}
  if (sp_enable(SP_FCR, SP_CLEAR_TRASMITTER_FIFO, true) != 0) {return -1;}
  if (sp_enable(SP_FCR, SP_CLEAR_RECEIVER_FIFO, true) != 0) {return -1;}

  return 0;
}

int sp_configure_end() {

  if (sp_enable(SP_FCR, SP_ENABLE_FIFO, false) ) {return -1;}
  if (sp_enable(SP_FCR, SP_CLEAR_RECEIVER_FIFO, true)) {return -1;}
  if (sp_enable(SP_FCR, SP_CLEAR_TRASMITTER_FIFO, true)) {return -1;}

  if (sp_enable(SP_IER, SP_RECEIVER_LINE_INTERRUPT, false) ) {return -1;}
  if (sp_enable(SP_IER, SP_TRANSMITTER_INTERRUPT, false) ) {return -1;}
  if (sp_enable(SP_IER, SP_RECEIVED_INTERRUPT, false)) {return -1;}  
  

  return 0;
}

int (sp_subscribe_int)(uint8_t *bit_no) {
  hook_id_sp = 4;
  *bit_no = hook_id_sp;

  if (sys_irqsetpolicy(SP_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id_sp) != 0) {return -1;}

  if (sp_configure_init() != 0) {return -1;}

  transmitQueue = createQueue(10);
  receiveQueue = createQueue(10);

  return 0;
}

int (sp_unsubscribe_int)() {
  if (sp_configure_end() != 0) {return -1;}

  if (sys_irqrmpolicy(&hook_id_sp) != 0) {return -1;}

  sp_clear();
  free(receiveQueue);

  while (!queueIsEmpty(transmitQueue)) {
    sp_write();
  }
  free(transmitQueue);

  return 0;
}

bool sp_check_write() {
  uint32_t lsr;

  if (sys_inb(SP_COM + SP_LSR, &lsr) != 0) {return -1;}

  if ((lsr & SP_TRANSMITTER_HOLDING_EMPTY) && ~(lsr & SP_RECEIVER))
    return true;
  else
    return false;
  
}

bool sp_check_read() {
  uint32_t lsr;

  if (sys_inb(SP_COM + SP_LSR, &lsr) != 0) {return false;}

  if (lsr & SP_RECEIVER)
    return true;
  else
    return false;
}

int sp_write() {
  uint8_t data = 0x00;

  if (!queueIsEmpty(transmitQueue) && sp_check_write()) {
    data = removeFromQueue(transmitQueue);
    if (sys_outb(SP_COM + SP_THR, data) != 0){return -1;}
    
    return 0;
  }
  else
    return -1;
}

int sp_read() {
  uint32_t rbr;


  if (sp_check_read()) {

    if (sys_inb(SP_COM + SP_RBR, &rbr) != 0) {return -1;}
    addToQueue(receiveQueue, (uint8_t)rbr);
  }else{
    
    return -1;
  }
  
  return 0;
}

void sp_emptyTransmitQueue(){
  while(queueIsEmpty(transmitQueue)){
    if(sp_check_write()){
      sp_write();
    }
  }
}

uint8_t (sp_ih)() {
  uint32_t interruptType = 0x00;
  uint8_t data = 0x00;

  if (sys_inb(SP_COM + SP_IIR, &interruptType) != 0) {return 0;}


  if (interruptType & SP_NO_PENDING_INTERRUPT)
    return 0;

  if (interruptType & SP_RECEIVED_DATA) {
    sp_read();
  }
  else if (interruptType & SP_TRANSMITTER_HOLDING) {
    sp_write();
  }

  return data;

}

uint8_t readFromQueue() {
  uint8_t data = 0x00;
  if (!queueIsEmpty(receiveQueue)) {
    data = removeFromQueue(receiveQueue);
  }

  return data;
}

void addToTransmitQueue(uint8_t data) {
  if (!queueIsFull(transmitQueue)) {
    addToQueue(transmitQueue, data);
  }
}


