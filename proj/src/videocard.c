#include "videocard.h"
#include "colors.h"
#include <string.h>


static char * video_mem;
char * video_mem_buffer;

vbe_mode_info_t vmi_p;
int bytes_per_pixel;
unsigned h_res;
unsigned v_res;
static unsigned int vram_size;


unsigned (get_h_res)(){
    return vmi_p.XResolution;
}

unsigned (get_v_res)(){
    return vmi_p.YResolution;
}

int (init_graphics_mode)(uint16_t mode){

  map_vram(&video_mem, 0, mode);
  map_vram(&video_mem_buffer, 1, mode);


  /* set mode*/
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

int (map_vram)(char ** video_mem_pointer, int n, uint16_t mode){
  struct minix_mem_range mr;
  int r;

  if((r=vbe_get_mode_info(mode,&vmi_p))!=OK)
    return r;


  bytes_per_pixel = ((vmi_p.BitsPerPixel + 7) >> 3);  //arredonda nr_bytes para cima

  vram_size = vmi_p.XResolution*vmi_p.YResolution*bytes_per_pixel;
  unsigned int vram_base = vmi_p.PhysBasePtr + n*vram_size;


  mr.mr_base=vram_base;
  mr.mr_limit=mr.mr_base+vram_size;

  if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
   panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */

  *video_mem_pointer = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(*video_mem_pointer == MAP_FAILED)
    panic("couldn't map video memory");

  return 0;
}

int (create_image)(xpm_map_t xpm, xpm_image_t *image){
  if(xpm_load(xpm, XPM_INDEXED, image) == NULL)
    return 1;
  return 0;
}


int (draw_hline)(char* board_mem,uint16_t 	x,uint16_t 	y,uint16_t 	len,uint32_t 	color ){

  for(unsigned i=x;i<x+len && i<vmi_p.XResolution;i++){    
    unsigned position = (y*vmi_p.XResolution + i )* bytes_per_pixel;
    memcpy((void*)((unsigned)board_mem + position),(void*) &color, bytes_per_pixel);
  }
  return 0;
}


int (draw_rectangle)(char* board_mem, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(unsigned i=y;i<y+height && i<vmi_p.YResolution;i++){
    draw_hline(board_mem, x,i,width,color);
  }
  return 0; 
}


int(draw_image)(char* board_mem,xpm_image_t img, uint16_t x, uint16_t y){
  unsigned color_transparent = COLOR_TRANSPARENT;

  unsigned color_position = 0; 
      
  for(int y_img = 0; y_img < img.height && y+y_img< vmi_p.YResolution; y_img++){ 
    unsigned position = ((y+y_img)* vmi_p.XResolution  + x ) * bytes_per_pixel; 
      
    for(int x_img = 0; x_img < img.width && x+x_img< vmi_p.XResolution; x_img++){
      position += bytes_per_pixel;
      color_position += bytes_per_pixel;

      if(memcmp(&img.bytes[color_position], &color_transparent, bytes_per_pixel) != 0 )
        memcpy((void*)((unsigned)board_mem + position), (void*)&img.bytes[color_position], bytes_per_pixel);
    }
    
  }
  return 0;
}


void (copy_from_buffer)(){
  copy_buffers(video_mem, video_mem_buffer);
}

void (copy_buffers)(void* vm1, void* vm2){
  memcpy(vm1, vm2, vram_size);
}

void vg_clear(){
  memset(video_mem_buffer, 0, vram_size);
}

int(vg_clear_image)(xpm_image_t img,uint16_t x,uint16_t y){
  unsigned color_transparent = COLOR_TRANSPARENT;

  for(int y_img = 0; y_img < img.height ; y_img++){ 
    for(int x_img = 0; x_img < img.width; x_img++){
      unsigned color_position = (y_img*img.height + x_img) * bytes_per_pixel; 
      unsigned position = ((y+y_img)* vmi_p.XResolution  + x + x_img) * bytes_per_pixel; 
      if(memcmp(&img.bytes[color_position], &color_transparent, bytes_per_pixel) != 0 )
        memset((void*)((unsigned)video_mem_buffer + position),0,bytes_per_pixel);
    }
  }
  return 0;
}
