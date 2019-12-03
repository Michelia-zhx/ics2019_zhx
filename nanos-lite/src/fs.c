#include "fs.h"
#include <klib.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
void segment_write(void *dest, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t read_offset;
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
  {"stdout", 0, 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void init_fs() {
  file_table[3].size = screen_width() * screen_height() * 4;
  // TODO: initialize the size of /dev/fb
  Log("hello");
}

int fs_close(int fd){
  return 0;
}

int fs_open(const char *pathname, int flags, int mode){
  for (int i=0; i<NR_FILES; i ++){
    if (strcmp(pathname, file_table[i].name) == 0){
      file_table[i].read_offset = 0;
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
  if (file_table[fd].read == NULL){
    printf("file_table[%d].read == NULL\n", fd);
    size_t read_len = len;
    if (file_table[fd].read_offset + len > file_table[fd].size)
      read_len = file_table[fd].size - file_table[fd].read_offset;
    printf("have computed read_len\n");
    ramdisk_read(buf, file_table[fd].read_offset, read_len);
    file_table[fd].read_offset += read_len;
    return read_len;
  }
  else{
    file_table[fd].read_offset += len;
    return file_table[fd].read(buf, file_table[fd].read_offset-len, len);
  }
}

size_t fs_write(int fd, const void *buf, size_t len){
  size_t write_len;
  if (file_table[fd].write == NULL)
    write_len = file_table[fd].write(buf, file_table[fd].disk_offset+file_table[fd].read_offset, len);
  else
    write_len = ramdisk_write(buf, file_table[fd].disk_offset+file_table[fd].read_offset, len);
  file_table[fd].read_offset += len;
  return write_len;
}

size_t fs_lseek(int fd, size_t offset, int whence){
  switch (whence) {
    case SEEK_SET:
      file_table[fd].read_offset = offset;
      break;
    case SEEK_CUR:
      file_table[fd].read_offset += offset;
      break;
    case SEEK_END:
      file_table[fd].read_offset = file_table[fd].size;
      break;  
    default:
      panic("Invalid whence.\n");
  }
  return file_table[fd].read_offset;
}