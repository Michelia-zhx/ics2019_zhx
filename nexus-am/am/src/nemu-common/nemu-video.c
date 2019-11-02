#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>

uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      int temp = inl(0x100);
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = (temp >> 16);
      info->height = (temp & 0x0000ffff);
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
      int count = 0;
      for (int i=0; i<ctl->h; ++i){
        for (int j=0; j<ctl->w; ++j){
          fb[(i+ctl->y)*screen_width()+(ctl->x+i)] = *(ctl->pixels + count);
          count ++;
        }
      }
      if (ctl->sync) {
        outl(SYNC_ADDR, 0);
      }
      return size;
    }
  }
  return 0;
}

void __am_vga_init() {
  int i;
  int size = screen_width() * screen_height();
  for (i = 0; i < size; i ++) fb[i] = i;
  draw_sync();
}
