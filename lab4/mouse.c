#include "mouse.h"

int kb_hook_id=1;
int timer_hook_id=2;
uint32_t count=0;
struct packet pp;
int i=0;
bool valid_packet=true;
bool update_count=false;


int (timer_subscribe_int)(uint8_t* bit_no){
  *bit_no = timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&timer_hook_id);
}

int (timer_unsubscribe_int)(){
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)(){
  count++;
}

int (mouse_subscribe_int)(uint8_t* bit_no){
  *bit_no = kb_hook_id;
  return sys_irqsetpolicy(MOUSE_IRQ_LINE,IRQ_REENABLE|IRQ_EXCLUSIVE,&kb_hook_id);
}

int (mouse_unsubscribe_int)(){
  return sys_irqrmpolicy(&kb_hook_id);
}

void (mouse_ih)(){
  uint8_t status;
  util_sys_inb(STAT_REG,&status);
  if((status&AUX)&&(status&OBF_FULL)){
    uint8_t byte;
    util_sys_inb(OUT_BUF,&byte);
    if(!(status&(TIMEOUT_ERR|PAR_ERR))){
        if(i==0 && !(byte&FIRST_BYTE)){
          i--; //should be first byte of packet but isnt, try again until BIT(3) is set
        }
        else{
          pp.bytes[i]=byte;
        }
        if(i==2){ //just finished reading a full packet
          if(!valid_packet)
            valid_packet=true;//no need to print invalid packet, reset to true for future reading
          else{
            pp.lb = pp.bytes[0]&LEFT_BUTTON;
            pp.mb = pp.bytes[0]&MIDDLE_BUTTON;
            pp.rb = pp.bytes[0]&RIGHT_BUTTON;
            pp.x_ov = pp.bytes[0]&X_OVFL;
            pp.y_ov = pp.bytes[0]&Y_OVFL;
            bool x_neg = pp.bytes[0] & X_SIGN;
            bool y_neg = pp.bytes[0] & Y_SIGN;
            int16_t delta_x=pp.bytes[1],delta_y=pp.bytes[2];
            


            if(x_neg){
              delta_x |= SIGN_EXTENSION;
            }
            if(y_neg){
              delta_y |= SIGN_EXTENSION;
            }

            pp.delta_x = delta_x;
            pp.delta_y = delta_y;
            mouse_print_packet(&pp);
            update_count=true;
            i=-1;
          }
        }
    }
    else
      valid_packet=false;
  
    i++;
  }
}

int (write_kbc_command)(uint8_t command){

  int counter=3;

  while(counter>0){
    uint8_t status;
    util_sys_inb(STAT_REG,&status);
    if(!(status&IBF_FULL)){
      return sys_outb(CTRL_REG,command);
    }
    tickdelay(micros_to_ticks(25000));
  }
  return -1;

}

int (read_kbc_return)(uint8_t* ret){
  int counter=3;

  while(counter>0){
    uint8_t status;
    util_sys_inb(STAT_REG,&status);
    if(status&OBF_FULL){
      return util_sys_inb(OUT_BUF,ret);
    }
    tickdelay(micros_to_ticks(25000));
  }
  return -1;


}

int (write_kbc_arg)(uint8_t arg){
  int counter=3;

  while(counter>0){
    uint8_t status;
    util_sys_inb(STAT_REG,&status);
    if(!(status&IBF_FULL)){
      return sys_outb(IN_BUF,arg);
    }
    tickdelay(micros_to_ticks(25000));
  }
  return -1;
}

int (mouse_disable_data_reporting)(){
  uint8_t ret;
  
  write_kbc_command(BYTE_TO_MOUSE);
  write_kbc_arg(DIS_DATA_REPORTING);
  read_kbc_return(&ret);

  if(ret==0xFA)
    return 0;
  else
    return -1;
}
