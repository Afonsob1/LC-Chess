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
  int return_value;

  switch(*command){
    case 0x20:
      return_value = write_kbc(*command);
      if(return_value)
        return return_value;
      else
        return read_kbc(command);
      break;
    case 0x60:
      return write_kbc_args(*command,args);
      break;
    case 0xAA:
      write_kbc(*command);
      read_kbc(&output);
      if(output==0x55)
        return 0;
      else if(output==0xFC)
        return 1;
      else
        return 2;
      break;
    case 0xAB:
      read_kbc(&output);
      return output;
      break;
    case 0xAD:
      return write_kbc(*command);
      break;
    case 0xAE:
      return write_kbc(*command);
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
    if(!(stat & KBC_OUT_BUF)) {
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
      printf("Wrote to the cmd reg!\n");
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




