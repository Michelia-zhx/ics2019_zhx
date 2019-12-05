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
  // printf("in events_read\n");
  int key = read_key();
  // if (key != 0) printf("key: %d\n", key);
  int down = 0;
  if (key & 0x8000) {
    down = 1;
    key ^= 0x8000;
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
  return (strlen(buf) <= len ? strlen(buf) : len);
  
 /*
  int key = read_key();
  // Log("%d\n",key);
  // Log("%d %d\n",offset,len);
  // if (len == 0)
  // {
  //   len = offset;
  //   offset = 0;
  // }
  char file_buf[30];
  // printf("%s\n",keyname[key]);
  if (key==0)
  {
    uint32_t up = uptime();
    sprintf(file_buf,"t %u\n",up);
    // printf("%s\n", file_buf);
    if (len > strlen(file_buf))
    {
      len = strlen(file_buf);
    }
    // Log("%d\n",len);
    memcpy(buf,(void*)(file_buf+offset),len);
  }
  else 
  {
    if (key > 0x8000)
    {
      // key.keydown
      key &= 0x7fff;
      sprintf(file_buf,"kd %s\n", keyname[key]);
      if (len > strlen(file_buf))
      {
        len = strlen(file_buf);
      }
      memcpy(buf,(void*)(file_buf+offset),len);
    }
    else 
    {
      sprintf(file_buf,"ku %s\n", keyname[key]);
      if (len > strlen(file_buf))
      {
        len = strlen(file_buf);
      }
      memcpy(buf,(void*)(file_buf+offset),len);

    }
  }
  return len;
  */
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();
  
  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
  sprintf(dispinfo,"WIDTH: %d, HEIGHT: %d\n",screen_width(),screen_height());
}
