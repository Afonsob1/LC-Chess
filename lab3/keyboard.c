#include "keyboard.h"

int kb_hook_id=1;
int timer_hook_id=4;
int counter=0;
uint8_t code;

int (keyboard_subscribe_int)(uint8_t* bit_no){
  *bit_no=kb_hook_id;
  return sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&kb_hook_id);
}

int (keyboard_unsubscribe_int)(){
  return sys_irqrmpolicy(&kb_hook_id);
}

int (keyboard_int_handler)(){
  uint8_t kbc_state;
  bool valid_values=false;
  int r;
  if((r=util_sys_inb(STAT_REG,&kbc_state))!=0)
    return r;
  if((kbc_state&OBF_FULL) && !(kbc_state & (PAR_ERR|TIMEOUT_ERR|AUX))){
    valid_values=true;
  }
  if((r=util_sys_inb(OUT_BUF,&code))!=0)
    return r;
  if(code==TWO_BYTES && valid_values){
    uint8_t bytes[2];
    bytes[0]=code;
    if((r=util_sys_inb(OUT_BUF,&code))!=0)
      return r;
    bytes[1]=code;
    return kbd_print_scancode(!(code&BREAK_CODE),2,bytes);
  }
  else if (valid_values){
    uint8_t bytes[1];
    bytes[0]=code;
    return kbd_print_scancode(!(code&BREAK_CODE),1,bytes);
  }
  else
    return OUT_BUF_EMPTY;
}

int (timer_subscribe_int)(uint8_t* bit_no){
  *bit_no=timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&timer_hook_id);
}

int (timer_unsubscribe_int)(){
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)(){
  counter ++;
}

int (read_command_byte)(uint8_t* command_byte){
  int r;
  if((r=issue_kbc_command(READ_KBC_CMD,NULL))!=0){
    return r;
  }
  return read_kbc_return(command_byte);
}

int (write_command_byte)(uint8_t command_byte){
  return issue_kbc_command(WRITE_KBC_CMD,&command_byte);
}

int (check_kbc)(){
  uint8_t return_value;
  int r;
  if((r=issue_kbc_command(CHECK_KBC,NULL))!=0)
    return r;
  if((r=read_kbc_return(&return_value))!=0)
    return r;
  if(return_value==KBC_OK)
    return 0;
  else
    return -1;
}

int (check_kb_interface)(){
  int r;
  uint8_t return_value;
  if((r=issue_kbc_command(CHECK_KBD_INTERFACE,NULL))!=0)
    return r;
  if((r=read_kbc_return(&return_value))!=0)
    return r;
  return return_value;
}

int (disable_kbd_interface)(){
  return issue_kbc_command(DISABLE_KBD_INTERFACE,NULL);
}

int (enable_kbd_interface)(){
  return issue_kbc_command(ENABLE_KBD_INTERFACE,NULL);
}

int (read_kbc_return)(uint8_t *data){
  int counter=3;
  int r;
  while(counter>0){
    uint8_t stat;
    if((r=util_sys_inb(STAT_REG,&stat))!=0){
      printf("util_sys_inb failed with: %d", r);
    }
    if(stat & OBF_FULL){
      if((r=util_sys_inb(OUT_BUF,data))!=0){
        printf("util_sys_inb failed with: %d", r);
      }
      if(stat&(PAR_ERR|TIMEOUT_ERR|AUX))
        return -1;
      else
        return 0;
    }
    counter--;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  printf("Timeout reading return\n");
  return -1;
}

int (issue_kbc_command)(uint8_t command,uint8_t* arg){
  int r=0,counter=3;
  while(counter>0){
    uint8_t stat;
    if((r=util_sys_inb(STAT_REG,&stat))!=0){
      printf("util_sys_inb failed with: %d", r);
    }
    if(!(stat&KBC_ST_IBF)){
      if((r=sys_outb(KBC_CMD_REG,command))!=0){
        return r;
      }
      if(arg==NULL){ //no args command
        return 0;
      }
      else{
        if((r=util_sys_inb(STAT_REG,&stat))!=0)
          return r;
        if(!(stat&KBC_ST_IBF)){
          if((r=sys_outb(KBC_IBF,*arg))!=0)
            return r;
          else{
            return 0;
          }
        }
        else{
          return -1;
        }
      }
    }
    counter--;
    tickdelay(micros_to_ticks(DELAY_US));
  }
  printf("IBF always full when writing command!\n");
  return -1;
}
