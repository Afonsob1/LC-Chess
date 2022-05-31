#include "videocard.h"

int kb_hook_id=4;
int timer_hook_id=1;
bool done_drawing=false;
uint8_t code;
int counter=0;
int num_frames=0;
static char *video_mem;		/* Process (virtual) address to which VRAM is mapped */
static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */
static unsigned redMaskSize;
static unsigned greenMaskSize;
static unsigned blueMaskSize;
static xpm_image_t image;
static uint16_t x_i;
static uint16_t current_x;
static uint16_t current_y;
static uint16_t y_i;
static uint16_t x_f;
static uint16_t y_f;
static int16_t speed_;
static uint8_t frRate;


int (init_graphics_mode)(uint16_t mode){
  reg86_t r;
  memset(&r,0,sizeof(r));
  r.intno = 0x10;
  r.ah = 0x4F;
  r.al = 0x02;
  r.bx = mode | BIT(14);
  if(sys_int86(&r)!=OK){
    printf("sys86 failed\n");
    return 1;
  }
  return 0;
}

int (map_vram)(uint16_t mode){
  struct minix_mem_range mr;
  vbe_mode_info_t vmi_p;
  int r;
  if((r=vbe_get_mode_info(mode,&vmi_p))!=0){
    printf("Get mode info failed \n");
    return 1;
  }
  unsigned int vram_base = vmi_p.PhysBasePtr;
  h_res = vmi_p.XResolution;
  v_res = vmi_p.YResolution;
  bits_per_pixel = vmi_p.BitsPerPixel;
  redMaskSize = vmi_p.RedMaskSize;
  greenMaskSize = vmi_p.GreenMaskSize;
  blueMaskSize = vmi_p.BlueMaskSize;
  unsigned int vram_size;
  unsigned int bytes_per_pixel = ((bits_per_pixel + 7)>>3);
  vram_size = h_res*v_res*bytes_per_pixel;
  mr.mr_base = (phys_bytes)vram_base;
  mr.mr_limit = mr.mr_base + vram_size;
  if((r=sys_privctl(SELF,SYS_PRIV_ADD_MEM,&mr))!=OK)
    {
      panic("sys_privtl failed\n");
  }
  video_mem = vm_map_phys(SELF,(void*)mr.mr_base,vram_size);
  if(video_mem == MAP_FAILED) panic("couldn't map video memory");
  return 0;
}


int(kb_subscribe_int)(uint8_t *bit_no){
  *bit_no=kb_hook_id;
  return sys_irqsetpolicy(KEYBOARD_IRQ,IRQ_REENABLE|IRQ_EXCLUSIVE,&kb_hook_id);
}

int(kb_unsubscribe_int)(){
  return sys_irqrmpolicy(&kb_hook_id);
}

int(timer_subscribe_int)(uint8_t *bit_no){
  *bit_no=timer_hook_id;
  return sys_irqsetpolicy(TIMER0_IRQ,IRQ_REENABLE,&timer_hook_id);
}

int(timer_unsubscribe_int)(){
  return sys_irqrmpolicy(&timer_hook_id);
}


void(timer_int_handler)(){
  counter++;
  if(counter%(60/frRate)==0){
    if(speed_<0){
      if(num_frames%(-speed_)==0){
        if(x_i==x_f){
          vg_clear_image(image,current_x,current_y-1);
          draw_image(image,current_x,current_y);
          if(current_y==y_f)
            done_drawing=true;
          else
            current_y++;
        }
        else if(y_i==y_f){
          vg_clear_image(image,current_x,current_x-1);
          draw_image(image,current_x,current_y);
          if(current_x==x_f)
            done_drawing=true;
          else
            current_x++;
        }
        num_frames++;
      }
      else{
        num_frames++;
      }
    }
    else{
      if(x_i==x_f){
        vg_clear_image(image,current_x,current_y-speed_);
        draw_image(image,current_x,current_y);
        if(current_y==y_f)
          done_drawing=true;
        current_y+=speed_;
        if(current_y>y_f)
          current_y=y_f;
      }
      else if(y_i==y_f){
        vg_clear_image(image,current_x-speed_,current_y);
        draw_image(image,current_x,current_y);
        if(current_x==x_f)
          done_drawing=true;
        current_x+=speed_;
        if(current_x>x_f)
          current_x=x_f;
      }
    }
  }
}

int (init_graphics)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf, int16_t speed, uint8_t fr_rate){
  map_vram(INDEXED_MODE);
  init_graphics_mode(INDEXED_MODE);
  xpm_load(xpm,XPM_INDEXED,&image);
  draw_image(image,xi,xf);
  if(xi==xf && yi==yf){
    done_drawing=true;
    return 0;
  }
  x_i=xi;
  x_f=xf;
  y_i=yi;
  y_f=yf;
  speed_=speed;
  frRate=fr_rate;
  if(speed>0 && xi==xf){
    current_x=xi;
    current_y=y_i+speed_;
    if(current_y>y_f)
      current_y=y_f;
  }
  else if(speed>0 && yi==yf){
    current_y=yi;
    current_x=x_i+speed_;
    if(current_x>x_f)
      current_x=x_f;
  }
  else if(speed<0 && xi==xf){
    current_x=xi;
    current_y=yi+1;
    num_frames++;
  }
  else if(speed<0 && yi==yf){
    current_y=yi;
    current_x=xi+1;
    num_frames++;
  }
  return 0;
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
  unsigned bytes_per_pixel = ((bits_per_pixel + 7) >>3);
  for(unsigned int i = x; i<x+len && i<h_res;i++){
    unsigned position =(y*h_res+i) * bytes_per_pixel;
    memcpy((void*)((unsigned)video_mem + position),(void *) &color,bytes_per_pixel);
  }
  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(unsigned int i=y;i<y+height && i<v_res;i++){
    vg_draw_hline(x,i,width,color);
  }
  return 0; 
}

int (vg_draw_rectangles)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
  vg_draw_rectangle((h_res/no_rectangles)*no_rectangles,0,h_res%no_rectangles,v_res,COLOR_BLACK);
  vg_draw_rectangle(0,(v_res/no_rectangles)*no_rectangles,h_res,v_res%no_rectangles,COLOR_BLACK);

  unsigned rectangle_width = h_res/no_rectangles;
  unsigned rectangle_height = v_res/no_rectangles;


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


int(draw_image)(xpm_image_t img,uint16_t x,uint16_t y){
  uint8_t* video_mem_8bit=(uint8_t*)video_mem;
  for(unsigned i=0;i<img.height && y+i<v_res;i++){
    for(unsigned j=0;j<img.width && x+j<h_res;j++){
        video_mem_8bit[(y+i)*h_res+(x+j)]=img.bytes[i*img.width+j];
    }
  }
  return 0;
}

int(vg_clear_image)(xpm_image_t img,uint16_t x,uint16_t y){
  uint8_t* video_mem_8bit=(uint8_t*)video_mem;
  for(unsigned i=0;i<img.height && y+i<v_res;i++){
    for(unsigned j=0;j<img.width && x+j<h_res;j++){
        video_mem_8bit[(y+i)*h_res+(x+j)]=0;
    }
  }
  return 0;
}
