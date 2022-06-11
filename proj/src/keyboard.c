#include "keyboard.h"


static int kb_hook_id=KB_IRQ;


int (keyboard_subscribe_int)(uint8_t* bit_no){
  *bit_no=kb_hook_id;
  return sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&kb_hook_id);
}

int (keyboard_unsubscribe_int)(){
  return sys_irqrmpolicy(&kb_hook_id);
}

int (keyboard_int_handler)(){
  uint8_t kbc_state;
  readKey = false;
  int r;


  // read kdc state
  if((r=util_sys_inb(STAT_REG,&kbc_state))!=0) return r;

  // check if read a valic scancode
  if((kbc_state&OBF_FULL) && !(kbc_state & (PAR_ERR|TIMEOUT_ERR|AUX))){
    // read scancode
    if((r=util_sys_inb(OUT_BUF,&scancode))!=0)
      return r;
    readKey = true;

    return 0;
  }

  return 1;
}


