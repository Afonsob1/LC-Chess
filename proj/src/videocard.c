#include "videocard.h"
#include "colors.h"
#include <string.h>


static void * video_mem;
vbe_mode_info_t vmi_p;
int bytes_per_pixel;


unsigned (get_h_res)(){
    return vmi_p.XResolution;
}

unsigned (get_v_res)(){
    return vmi_p.YResolution;
}

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
  int r;

  if((r=vbe_get_mode_info(mode,&vmi_p))!=OK)
    return r;


  bytes_per_pixel = ((vmi_p.BitsPerPixel + 7) >> 3);  //arredonda nr_bytes para cima

  unsigned int vram_base = vmi_p.PhysBasePtr;
  unsigned int vram_size = vmi_p.XResolution*vmi_p.YResolution*bytes_per_pixel;


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

int (create_image)(xpm_map_t xpm, xpm_image_t *image){
  if(xpm_load(xpm, XPM_INDEXED, image) == NULL)
    return 1;
  return 0;
}


int (vg_draw_hline)(uint16_t 	x,uint16_t 	y,uint16_t 	len,uint32_t 	color ){

  for(unsigned i=x;i<x+len && i<vmi_p.XResolution;i++){    
    unsigned position = (y*vmi_p.XResolution + i )* bytes_per_pixel;
    memcpy((void*)((unsigned)video_mem + position),(void*) &color, bytes_per_pixel);
  }
  return 0;
}


int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for(unsigned i=y;i<y+height && i<vmi_p.YResolution;i++){
    vg_draw_hline(x,i,width,color);
  }
  return 0; 
}


int(vg_draw_image)(xpm_image_t img, uint16_t x, uint16_t y){
  unsigned color_transparent = COLOR_TRANSPARENT;

  for(int y_img = 0; y_img < img.height ; y_img++){ 
    for(int x_img = 0; x_img < img.width; x_img++){
      unsigned position = ((y+y_img)* vmi_p.XResolution  + x + x_img) * bytes_per_pixel; 
      unsigned color_position = (y_img*img.height + x_img) * bytes_per_pixel; 
      
      if(memcmp(&img.bytes[color_position], &color_transparent, bytes_per_pixel) != 0 )
        memcpy((void*)((unsigned)video_mem + position), (void*)&img.bytes[color_position], bytes_per_pixel);
    }
    
  }
  return 0;
}

int(vg_clear_image)(xpm_image_t img,uint16_t x,uint16_t y){
  uint8_t* video_mem_8bit=(uint8_t*)video_mem;
  for(unsigned i=0;i<img.height && y+i<vmi_p.YResolution;i++){
    for(unsigned j=0;j<img.width && x+j<vmi_p.XResolution;j++){
        video_mem_8bit[(y+i)*vmi_p.XResolution+(x+j)]=0;
    }
  }
  return 0;
}
