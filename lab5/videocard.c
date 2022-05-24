#include "videocard.h"


int counter=0;
int kb_hook_id=4;
int timer_hook_id=1;
uint8_t code;
uint8_t bytes_per_pixel;
bool done=false;
vbe_mode_info_t vmi_p;
uint8_t *video_mem;

int (set_vbe_mode)(uint16_t mode){
  reg86_t r;
  memset(&r,0,sizeof(r));
  r.ax = 0x4F02; //VBE call, function 02
  r.bx = mode | LINEAR_FRAMEBUFFER;
  r.intno = VIDEO_CARD_INT;
  if(sys_int86(&r)!=OK){
    printf("set_vbe_mode: sys_int_86() failed\n");
    return 1;
  }
  return 0;
}

int(kb_subscribe_int)(uint8_t *bit_no){
  *bit_no=kb_hook_id;
  return sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&kb_hook_id);
}

int(kb_unsubscribe_int)(){
  return sys_irqrmpolicy(&kb_hook_id);
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t aux;
  int r;
  if((r=sys_inb(port,&aux))!=0)
    return r;
  *value = (uint8_t) aux&LSB_MASK;
  return 0;   
}


int(kb_int_handler)(){
  uint8_t status;
  util_sys_inb(STATUS_REG,&status);
  if(status&OUT_BUF_FULL)
    util_sys_inb(OUT_BUF,&code);
  if(status & (AUX|PARITY_ERR|TIMEOUT_ERR))
    return -1;
  if(code==TWO_BYTES){
    if(status&OUT_BUF_FULL)
      util_sys_inb(OUT_BUF,&code);
    if(status & (AUX|PARITY_ERR|TIMEOUT_ERR))
      return -1;  
  }
  return 0;
}





int (map_vram)(uint16_t mode){
  vbe_get_mode_info(mode,&vmi_p);
  int r;
  struct minix_mem_range mr; /* physical memory range */

  bytes_per_pixel = (vmi_p.BitsPerPixel+7)>>3;

  unsigned int vram_base = vmi_p.PhysBasePtr;
  unsigned int vram_size = bytes_per_pixel*vmi_p.XResolution*vmi_p.YResolution;

  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
    panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */
  video_mem = (uint8_t*)vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if(video_mem == MAP_FAILED)
    panic("couldn’t map video memory");
  return 0;
}

int (vg_draw_hline)(uint16_t x,uint16_t y, uint16_t len,uint32_t color){
  for(int i=x;i<x+len && i <vmi_p.XResolution;i++)
    memcpy(&video_mem[(y*vmi_p.XResolution+i)*bytes_per_pixel],&color,bytes_per_pixel);

  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color){
  for(int col=y;col<y+height && col<vmi_p.YResolution;col++){
    vg_draw_hline(x,col,width,color);
  }
  return 0;
}

int (vg_draw_matrix)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){

  uint32_t rectangle_width = vmi_p.XResolution/no_rectangles;
  uint32_t rectangle_height = vmi_p.YResolution/no_rectangles;

  uint16_t blackstripe_width = vmi_p.XResolution%no_rectangles;
  uint16_t blackstripe_height = vmi_p.YResolution%no_rectangles;

  vg_draw_rectangle(vmi_p.XResolution-blackstripe_width,0,blackstripe_width,vmi_p.YResolution,COLOR_BLACK);
  vg_draw_rectangle(0,vmi_p.YResolution-blackstripe_height,vmi_p.XResolution,blackstripe_height,COLOR_BLACK);

  uint32_t color,r_color,g_color,b_color;

  uint32_t r_first = (first>>(vmi_p.GreenMaskSize+vmi_p.BlueMaskSize))%BIT(vmi_p.RedMaskSize);

  uint32_t g_first = (first>>vmi_p.BlueMaskSize)%BIT(vmi_p.GreenMaskSize);

  uint32_t b_first = first%BIT(vmi_p.BlueMaskSize);


  for(uint8_t row=0;row<no_rectangles;row++){
    for(uint8_t col=0;col<no_rectangles;col++){
      if(mode==INDEXED_MODE){
        color = (first + (row*no_rectangles+col)*step)%BIT(vmi_p.BitsPerPixel);
      }
      else{
        r_color = (r_first+col*step)%BIT(vmi_p.RedMaskSize);
        g_color = (g_first+row*step)%BIT(vmi_p.GreenMaskSize);
        b_color = (b_first+(col+row)*step)%BIT(vmi_p.BlueMaskSize);

        color = r_color<<(vmi_p.GreenMaskSize+vmi_p.BlueMaskSize)|g_color<<vmi_p.BlueMaskSize|b_color;

      }
      vg_draw_rectangle(col*rectangle_width,row*rectangle_height,rectangle_width,rectangle_height,color);
    }
  }
  return 0;
}

int(vg_draw_image)(uint16_t x, uint16_t y, xpm_image_t img){
  for(int col=0;col<img.width;col++){
    for(int row=0;row<img.height;row++){
      memcpy(&video_mem[((y+row)*vmi_p.XResolution+col+x)*bytes_per_pixel],&img.bytes[row*img.width+col],bytes_per_pixel);
    }
  }
  return 0;
}


int (timer_subscribe_int)(uint8_t * bit_no){
  *bit_no=timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&timer_hook_id);
}

int (timer_unsubscribe_int)(){
  return sys_irqrmpolicy(&timer_hook_id);
}

void (timer_int_handler)(){
  counter++;
}

int (vg_clear_image)(uint16_t x, uint16_t y, xpm_image_t img){
  for(uint16_t row=y;row<y+img.height;row++){
      memset(&video_mem[(row)*vmi_p.XResolution*bytes_per_pixel+x],0,bytes_per_pixel*img.width);
  }
  return 0;
}
