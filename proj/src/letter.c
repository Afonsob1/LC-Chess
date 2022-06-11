

#include "letter.h"
#include <lcom/lcf.h>
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


xpm_image_t letters[26];

void initLetter(){
    create_image(a,&letters[0]);
    create_image(b,&letters[1]);
    create_image(c,&letters[2]);
    create_image(d,&letters[3]);
    create_image(e,&letters[4]);
    create_image(f,&letters[5]);
    create_image(g,&letters[6]);
    create_image(h,&letters[7]);
    create_image(i_,&letters[8]);
    create_image(j,&letters[9]);
    create_image(k,&letters[10]);
    create_image(l,&letters[11]);
    create_image(m,&letters[12]);
    create_image(n,&letters[13]);
    create_image(o,&letters[14]);
    create_image(p,&letters[15]);
    create_image(q,&letters[16]); 
    create_image(r,&letters[17]);
    create_image(s,&letters[18]);
    create_image(t,&letters[19]);
    create_image(u,&letters[20]);
    create_image(v,&letters[21]);
    create_image(w,&letters[22]);
    create_image(x,&letters[23]);
    create_image(y,&letters[24]);
    create_image(z,&letters[25]);
}

xpm_image_t* getLetter(uint8_t byte){
  switch (byte)
  {
  case LETTER_A | BIT(7): return &letters[0];
  case LETTER_B | BIT(7): return &letters[1];
  case LETTER_C | BIT(7): return &letters[2];
  case LETTER_D | BIT(7): return &letters[3];
  case LETTER_E | BIT(7): return &letters[4];
  case LETTER_F | BIT(7): return &letters[5];
  case LETTER_G | BIT(7): return &letters[6];
  case LETTER_H | BIT(7): return &letters[7];
  case LETTER_I | BIT(7): return &letters[8];
  case LETTER_J | BIT(7): return &letters[9];
  case LETTER_K | BIT(7): return &letters[10];
  case LETTER_L | BIT(7): return &letters[11];
  case LETTER_M | BIT(7): return &letters[12];
  case LETTER_N | BIT(7): return &letters[13];
  case LETTER_O | BIT(7): return &letters[14];
  case LETTER_P | BIT(7): return &letters[15];
  case LETTER_Q | BIT(7): return &letters[16];
  case LETTER_R | BIT(7): return &letters[17];
  case LETTER_S | BIT(7): return &letters[18];
  case LETTER_T | BIT(7): return &letters[19];
  case LETTER_U | BIT(7): return &letters[20];
  case LETTER_V | BIT(7): return &letters[21];
  case LETTER_W | BIT(7): return &letters[22];
  case LETTER_X | BIT(7): return &letters[23];
  case LETTER_Y | BIT(7): return &letters[24];
  case LETTER_Z | BIT(7): return &letters[25];
  default:
    return NULL;
  }
}

void draw_name(char * video_mem_buffer, uint8_t* make_code,  int base_x ,int base_y, int spacing){
    int x = 0;
    uint8_t* p = &(make_code[0]);
    int c = 0;

    while(*p!=0){
        x = base_x + (spacing * c);
        xpm_image_t *img = getLetter(*p);
        if(img){
            draw_image(video_mem_buffer, *img , x, base_y);
        }
        c++;
        p++;
    }
}

