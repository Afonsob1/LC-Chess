#include <mouse.h>

int mouse_hook_id=1,timer_hook_id=2;
unsigned packet_counter=0;
unsigned timer_counter=0;
bool valid_packet=true;
uint8_t i=0;
static uint8_t bytes[3];

int mouse_subscribe_int(uint8_t* bit_no){
  *bit_no=mouse_hook_id;
  return sys_irqsetpolicy(MOUSE_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&mouse_hook_id);
}

int mouse_unsubscribe_int(){
  return sys_irqrmpolicy(&mouse_hook_id);
}

int (timer_subscribe_int)(uint8_t* bit_no){
  *bit_no=timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&timer_hook_id);
}

int (timer_unsubscribe_int)(){
  return sys_irqrmpolicy(&timer_hook_id);
}

void timer_ih(){
  timer_counter++;
}

void (mouse_ih)(){
  uint8_t status;
  util_sys_inb(STAT_REG,&status);
  if(STAT_REG & (OBF_FULL | AUX)){
    util_sys_inb(OUT_BUFF,bytes+i);
    if(status&(PAR_ERR|TIMEOUT_ERR))
      valid_packet=false;
    if(i==2){
      if(valid_packet){
        uint8_t data_packet = bytes[0];
        bool rb_pressed = data_packet & RB_PRESSED;
        bool mb_pressed = data_packet & MB_PRESSED;
        bool lb_pressed = data_packet & LB_PRESSED;
        bool x_ov = data_packet & X_OVFL;
        bool y_ov = data_packet & Y_OVFL;
        bool x_neg = data_packet & X_SIGN;
        bool y_neg = data_packet & Y_SIGN;
        int16_t delta_x = (int16_t)bytes[1];
        if(x_neg)
          delta_x|=(int16_t)((uint16_t)-1-((uint16_t)BIT(8)-1));
        int16_t delta_y = (int16_t)bytes[2];
        if(y_neg)
          delta_y|=(int16_t)((uint16_t)-1-((uint16_t)BIT(8)-1));
        struct packet pp;
        pp.bytes[0]=bytes[0];
        pp.bytes[1]=bytes[1];
        pp.bytes[2]=bytes[2];
        pp.delta_x=delta_x;
        pp.delta_y=delta_y;
        pp.lb = lb_pressed;
        pp.mb = mb_pressed;
        pp.rb = rb_pressed;
        pp.x_ov = x_ov;
        pp.y_ov = y_ov;
        mouse_print_packet(&pp);
        packet_counter++;
      }
      else
        valid_packet=true;
      i=-1;
    }
    i++;
  }
}

/*
int (mouse_enable_data_reporting)(){
  uint8_t ack_byte;
  uint8_t command = EN_DATA_REPORTING;
  issue_kbc_command(BYTE_TO_MOUSE,&command);
  read_kbc_return(&ack_byte);
  if(ack_byte==ACK_BYTE_OK)
    return 0;
  else
    return -1;
}
*/


int (mouse_disable_data_reporting)(){
  uint8_t ack_byte;
  uint8_t command = DIS_DATA_REPORTING;
  issue_kbc_command(BYTE_TO_MOUSE,&command);
  read_kbc_return(&ack_byte);
  if(ack_byte==ACK_BYTE_OK)
    return 0;
  else
    return -1;
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
      if((r=util_sys_inb(OUT_BUFF,data))!=0){
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
          if((r=sys_outb(KBC_ST_IBF,*arg))!=0)
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
  return -1;
}
