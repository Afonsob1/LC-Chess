#include "videocard.h"

int hook_id=4;
uint8_t code;

static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned redMaskSize;
static unsigned greenMaskSize;
static unsigned blueMaskSize;


int (init_graphics_mode)(uint16_t mode){
  reg86_t r;
  memset(&r,0,sizeof(r));
  r.intno=0x10;
  r.ax=0x4F02;
  r.bx=BIT(14)|mode;
  if( sys_int86(&r) != OK ) {
    printf("vg_exit(): sys_int86() failed \n");
    return 1;
  }
  return 0;
}

int (return_text_mode)(){
  reg86_t r;
  memset(&r,0,sizeof(r));
  r.intno=0x10;
  r.ah=0x00;
  r.al=0x03;
  if( sys_int86(&r) != OK ) {
    printf("vg_exit(): sys_int86() failed \n");
    return 1;
  }
  return 0;
}

int (map_vram)(uint16_t mode){
  struct minix_mem_range mr;
  vbe_mode_info_t vmi_p;
  int r;

  if((r=vbe_get_mode_info(mode,&vmi_p))!=OK)
    return r;

  unsigned int vram_base = vmi_p.PhysBasePtr;

  h_res = vmi_p.XResolution;
  v_res = vmi_p.YResolution;
  bits_per_pixel = vmi_p.BitsPerPixel;
  redMaskSize = vmi_p.RedMaskSize;
  greenMaskSize = vmi_p.GreenMaskSize;
  blueMaskSize = vmi_p.BlueMaskSize;

  unsigned int vram_size = (h_res*v_res*bits_per_pixel)/8;


  mr.mr_base=vram_base;
  mr.mr_limit=mr.mr_base+vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
   panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED)
    panic("couldn't map video memory");
  return 0;
}


int(kb_subscribe_int)(uint8_t *bit_no){
  *bit_no=hook_id;
  return sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&hook_id);
}

int(kb_unsubscribe_int)(){
  return sys_irqrmpolicy(&hook_id);
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

int (vg_draw_hline)(uint16_t 	x,uint16_t 	y,uint16_t 	len,uint32_t 	color ){
  unsigned bytes_per_pixel = bits_per_pixel/8;
  if(bits_per_pixel%8>0)
    bytes_per_pixel++;
  for(unsigned i=x;i<x+len && i<h_res;i++){
    video_mem[(y*h_res+i)*bytes_per_pixel]=color;
  }
  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(unsigned i=y;i<y+height && i<v_res;i++){
    vg_draw_hline(x,i,width,color%BIT(bits_per_pixel));
  }
  return 0; 
}

int (vg_draw_rectangles)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
  vg_draw_rectangle((h_res/no_rectangles)*no_rectangles,0,h_res%no_rectangles,v_res,COLOR_BLACK);
  vg_draw_rectangle(0,(v_res/no_rectangles)*no_rectangles,h_res,v_res%no_rectangles,COLOR_BLACK);

  unsigned rectangle_width = h_res/no_rectangles;
  unsigned rectangle_height = v_res/no_rectangles;


  first%=BIT(bits_per_pixel);

  uint32_t red_first = (first>>(greenMaskSize+blueMaskSize))%BIT(redMaskSize);
  uint32_t green_first = (first>>blueMaskSize)%BIT(greenMaskSize);
  uint32_t blue_first = first % BIT(blueMaskSize);

  for(in_port_t row=0;row<no_rectangles;row++){
    for(in_port_t col=0;col<no_rectangles;col++){
      gid_t color;
      if(mode==0x105){ //indexed mode
        color = (first + (row * no_rectangles + col) * step) % (1 << bits_per_pixel);
      }
      else{ //rgb
        uint32_t red = (red_first + col * step) % BIT(redMaskSize) ;
        uint32_t green = (green_first + row * step) % BIT(greenMaskSize);
        uint32_t blue = (blue_first + (col + row) * step) % BIT(blueMaskSize);
        red = red << (greenMaskSize+blueMaskSize);
        green = green << blueMaskSize; 
        color = red | green | blue;
      }
      vg_draw_rectangle(col*rectangle_width,row*rectangle_height,
      rectangle_width,rectangle_height,color);
    }
  }
  return 0;
}
