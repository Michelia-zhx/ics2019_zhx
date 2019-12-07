#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t remain_len = len;
  while (remain_len){
    _putc(*((char *)buf));
    buf ++;
    remain_len --;
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  // /*
  // printf("in events_read\n");
  int key = read_key();
  // if (key != 0) printf("key: %d\n", key);
  int down = 0;
  if (key & 0x8000) {
    down = 1;
    key &= 0x7fff;
  }
  else down = 0;
  // key = key & 0x7fff;
  if (key == _KEY_NONE) {
    uint32_t cur_time = uptime();
    sprintf(buf, "t %d\n", cur_time);
  }
  else {
    if(down) sprintf(buf,"kd %s\n",keyname[key]);
  	else sprintf(buf,"ku %s\n",keyname[key]);
  }
  return (strlen(buf) < len ? strlen(buf) : len);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  memcpy(buf, dispinfo + offset, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int width = screen_width();
  offset = offset / sizeof(uint32_t);
  draw_rect((uint32_t *)buf, offset % width, offset/width, len/4, 4);
  return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  // Log("WIDTH:%d, HEIGHT:%d\n",screen_width(),screen_height());
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo,"WIDTH :%d\nHEIGHT :%d",screen_width(),screen_height());
}
