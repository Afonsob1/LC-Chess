#include "keyboard.h"
#include "videocard.h"
#include "imgs/letters/a.h"
#include "imgs/letters/b.h"
#include "imgs/letters/c.h"
#include "imgs/letters/d.h"
#include "imgs/letters/e.h"
#include "imgs/letters/f.h"
#include "imgs/letters/g.h"
#include "imgs/letters/h.h"
#include "imgs/letters/i_.h"
#include "imgs/letters/j.h"
#include "imgs/letters/k.h"
#include "imgs/letters/l.h"
#include "imgs/letters/m.h"
#include "imgs/letters/n.h"
#include "imgs/letters/o.h"
#include "imgs/letters/p.h"
#include "imgs/letters/q.h"
#include "imgs/letters/r.h"
#include "imgs/letters/s.h"
#include "imgs/letters/t.h"
#include "imgs/letters/u.h"
#include "imgs/letters/v.h"
#include "imgs/letters/w.h"
#include "imgs/letters/x.h"
#include "imgs/letters/y.h"
#include "imgs/letters/z.h"


int kb_hook_id=4;
int counter=0;
uint8_t code;
xpm_image_t letterImg;

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
    drawLetter(bytes[1]);
  }
  else if (valid_values){
    uint8_t bytes[1];
    bytes[0]=code;
    drawLetter(bytes[0]);
  }
  else
    return OUT_BUF_EMPTY;
  return 0;
}

void drawLetter(uint8_t byte){
  switch (byte)
  {
  case 0x1e:
    create_image(a,&letterImg);
    break;
  case 0x30: 
    create_image(b,&letterImg);
    break;
  case 0x2e:
    create_image(c,&letterImg);
    break;
  case 0x20:
    create_image(d,&letterImg);
    break;
  case 0x12:
    create_image(e,&letterImg);
    break;
  case 0x21:
    create_image(f,&letterImg);
    break;
  case 0x22:
    create_image(g,&letterImg);
    break;
  case 0x23:
    create_image(h,&letterImg);
    break;
  case 0x17:
    create_image(i_,&letterImg);
    break;
  case 0x24:
    create_image(j,&letterImg);
    break;
  case 0x25:
    create_image(k,&letterImg);
    break;
  case 0x26:
    create_image(l,&letterImg);
    break;
  case 0x32:
    create_image(m,&letterImg);
    break;
  case 0x31:
    create_image(n,&letterImg);
    break;
  case 0x18:
    create_image(o,&letterImg);
    break;
  case 0x19:
    create_image(p,&letterImg);
    break;
  case 0x10:
    create_image(q,&letterImg); 
    break;
  case 0x13:
    create_image(r,&letterImg);
    break;
  case 0x1f:
    create_image(s,&letterImg);
    break;
  case 0x14:
    create_image(t,&letterImg);
    break;
  case 0x16:
    create_image(u,&letterImg);
    break;
  case 0x2f: 
    create_image(v,&letterImg);
    break;
  case 0x11: 
    create_image(w,&letterImg);
    break;
  case 0x2d:
    create_image(x,&letterImg);
    break;
  case 0x15:
    create_image(y,&letterImg);
    break;
  case 0x2c: 
    create_image(z,&letterImg);
    break;
  default:
    break;
  }
}

