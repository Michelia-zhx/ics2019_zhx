#include "fs.h"
#include <klib.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
void segment_write(void *dest, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);
size_t fbsync_write(const void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, 0, invalid_read, serial_write},
  {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
  {"/proc/dispinfo", 30, 0, 0, dispinfo_read, NULL},
  {"/dev/fdsync", 0, 0, 0, NULL, fbsync_write},
  {"/dev/events", 0, 0, 0, events_read, NULL},
  {"/dev/tty", 0, 0, 0, NULL, serial_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  int vga_id = fs_open("/dev/fb",0,0);
  file_table[vga_id].size = screen_width() * screen_height()*sizeof(uint32_t);
  // TODO: initialize the size of /dev/fb
  Log("screen_width: %d, screen_height: %d\n", screen_width, screen_width);
}

int fs_open(const char *pathname, int flags, int mode){
  for (int i=0; i<NR_FILES; i ++){
    if (strcmp(pathname, file_table[i].name) == 0){
      file_table[i].open_offset = 0;
      return i;
    }
  }
  panic("pathname: %s -- File not found.\n", pathname);
  return -1;
}

size_t fs_filesz(int fd){
  if (fd <= NR_FILES)
    return file_table[fd].size;
  else
    panic("No such file in file_table.\n");
  
}

size_t fs_read(int fd, void *buf, size_t len){
  /*
  if (file_table[fd].read == NULL){
    // printf("file_table[%d].read == NULL\n", fd);
    size_t read_len = len;
    // printf("len: %d, file_table[%d].size: %d, file_table[fd].read_offset: %d.\n", len, fd, file_table[fd].size, file_table[fd].read_offset);
    // printf("file_table[%d].disk_offset: %d.\n", fd, file_table[fd].disk_offset);
    if (file_table[fd].open_offset + len > file_table[fd].size)
      read_len = file_table[fd].size - file_table[fd].open_offset;
    ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, read_len);
    // printf("have done ramdisk_read.\n");
    file_table[fd].open_offset += read_len;
    return read_len;
  }
  else{
    file_table[fd].open_offset += len;
    return file_table[fd].read(buf, file_table[fd].open_offset-len, len);
  }
  */
  if (file_table[fd].read == events_read && file_table[fd].open_offset == file_table[fd].size){
    file_table[fd].open_offset = 0;
  }
  
  if (file_table[fd].open_offset + len > file_table[fd].size) {
    len = file_table[fd].size - file_table[fd].open_offset;
  }
  if (file_table[fd].read!=NULL) {
    size_t ret = file_table[fd].read(buf,file_table[fd].open_offset,len);
    file_table[fd].open_offset+=len;
    return ret;
  }
  ramdisk_read(buf,file_table[fd].disk_offset + file_table[fd].open_offset,len);
  file_table[fd].open_offset+=len;
  // Log("%d\n",file(fd).open_offset);
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len){
  if (fd==1 || fd==2){
    char *addr = (char *)(buf);
    uintptr_t count = len;
    while (count){
      _putc(*addr);
      addr ++;
      count --;
    }
    return len;
  }
  else{
    int write_len = len;
    if (file_table[fd].size < file_table[fd].open_offset + len)
      write_len = file_table[fd].size - file_table[fd].open_offset;

    if (file_table[fd].write != NULL)
      write_len = file_table[fd].write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, write_len);
    else
      write_len = ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].open_offset, write_len);

    file_table[fd].open_offset += write_len;
    return write_len;
  }
}

size_t fs_lseek(int fd, size_t offset, int whence){
  switch (whence) {
    case SEEK_SET:
      file_table[fd].open_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;
    case SEEK_END:
      file_table[fd].open_offset = file_table[fd].size;
      break;  
    default:
      panic("Invalid whence.\n");
  }
  return file_table[fd].open_offset;
}

int fs_close(int fd){
  file_table[fd].open_offset = 0;
  return 0;
}