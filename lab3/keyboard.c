#include <keyboard.h>


void (kbc_ih)(){
  uint8_t status;
  util_sys_inb(STAT_REG,&status);
  if(!(status&0x1))
    return;
  util_sys_inb(OUTPUT_BUFFER,&c);
  if(c==0xe0){
    uint8_t bytes[2];
    bytes[0]=c;
    util_sys_inb(OUTPUT_BUFFER,&c);
    bytes[1]=c;
    kbd_print_scancode(!(c&0x80),2,bytes);
  }
  else{
    uint8_t bytes[1];
    bytes[0]=c;
    kbd_print_scancode(!(c&0x80),1,bytes);
  }
}

int (issue_command)(uint8_t* command,uint8_t args){
  uint8_t output;
  switch(*command){
    case ReadCMDByte:
      write_kbc(ReadCMDByte);
      return read_kbc(command);
      break;
    case WriteCMDByte:
      return write_kbc_args(WriteCMDByte,args);
      break;
    case checkKBC:
      write_kbc(checkKBC);
      read_kbc(&output);
      if(output==KBCTestOK)
        return 0;
      else if(output==KBCTestFailed)
        return 1;
      else
        return 2;
      break;
    case CheckKBInterface:
      read_kbc(&output);
      return output;
      break;
    case DisableKBDInterface:
      return write_kbc(DisableKBDInterface);
      break;
    case EnableKBDInterface:
      return write_kbc(EnableKBDInterface);
      break;
    default:
      return 2;
  }
}

int (read_kbc)(uint8_t *kbc_status){
  uint8_t counter = 3;
  while(counter>0) {
    uint8_t stat;
    util_sys_inb(KBC_ST_REG, &stat); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    if(stat & KBC_OUT_BUF) {
        util_sys_inb(OUTPUT_BUFFER,kbc_status);
        if(!(stat & (KBC_PAR_ERROR |KBC_TO_ERROR)))
          return EXIT_SUCCESS;
        else
          return EXIT_FAILURE;
    }
    counter--;
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
  }
  return -1;
}

int (write_kbc)(uint8_t command){
  uint8_t counter = 3;
  while(counter>0) {
    uint8_t stat;
    util_sys_inb(KBC_ST_REG, &stat); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    if( (stat & KBC_ST_IBF) == 0 ) {
      sys_outb(KBC_CMD_REG, command);
      return 0;
    }
    counter--;
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
  }
  return -1;
}

int (write_kbc_args)(uint8_t command,uint8_t args){
  uint8_t counter = 3;
  while(counter>0) {
    uint8_t stat;
    util_sys_inb(KBC_ST_REG, &stat); /* assuming it returns OK */
    /* loop while 8042 input buffer is not empty */
    if( (stat & KBC_ST_IBF) == 0 ) {
      sys_outb(KBC_CMD_REG, command);
      sys_outb(KBC_INPUT_BUFFER, args);
      return 0;
    }
    counter--;
    tickdelay(micros_to_ticks(DELAY_US)); // e.g. tickdelay()
  }
  return -1;
}


void (timer_int_handler)(){
  counter--;
}




