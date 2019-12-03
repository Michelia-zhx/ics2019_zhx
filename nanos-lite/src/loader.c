#include "proc.h"
#include <elf.h>
#include <klib.h>
#include "fs.h"

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_off Elf64_Off
# define Elf_Half Elf64_Half
# define Elf_Word Elf64_Word
# define Elf_Addr Elf64_Addr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_off Elf32_Off
# define Elf_Half Elf32_Half
# define Elf_Word Elf32_Word
# define Elf_Addr Elf32_Addr
#endif

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  // printf("In loader.c\n");
  int file_index;
  if (filename == NULL) file_index = 23;
  else file_index = fs_open(filename, 0, 0);

  Elf_Ehdr ehdr;
  fs_read(file_index, &ehdr, sizeof(Elf_Ehdr));

  Elf_off phdr_offset = ehdr.e_phoff;
  // printf("phdr_offset: %d\n", phdr_offset);
  Elf_Half phdr_size = ehdr.e_phentsize;
  // printf("phdr_size: %d\n", phdr_size);
  Elf_Half phdr_num = ehdr.e_phnum;
  // printf("phdr_num: %d\n", phdr_num);

  for (int i=0; i<phdr_num; i ++) {
    Elf_Phdr phdr;
    // printf("phdr_offset+i*phdr_size: %d\n", phdr_offset+i*phdr_size);
    fs_lseek(file_index, phdr_offset + i*phdr_size, SEEK_SET);
    fs_read(file_index, &phdr, phdr_size);
    // printf("phdr.type: %d\n", phdr.p_type);
    // printf("p_offset: %d\n", phdr.p_offset);
    // printf("p_filesize: %d\n", phdr.p_filesz);
    // printf("p_memsize: %d\n", phdr.p_memsz);
    switch (phdr.p_type) {
      case PT_LOAD: {
        fs_lseek(file_index, phdr.p_offset, SEEK_SET);
        fs_read(file_index, (void *)phdr.p_vaddr, phdr.p_filesz);
        memset((void *)(phdr.p_vaddr+phdr.p_filesz), 0, phdr.p_memsz-phdr.p_filesz);
        break;
      }
      default:
        break;
    }
  }
  fs_close(file_index);
  return ehdr.e_entry;
}

/*p_offset  This  member holds the offset from the beginning of
the file at which the first byte of the segment resides.
p_vaddr   This member holds the  virtual  address  at  which
the first byte of the segment resides in memory.
其中相对文件偏移Offset指出相应segment的内容从ELF文件的第Offset字节开始,
在文件中的大小为FileSiz, 它需要被分配到以VirtAddr为首地址的虚拟内存位置,
在内存中它占用大小为MemSiz. 也就是说, 这个segment使用的内存就是
[VirtAddr, VirtAddr + MemSiz)这一连续区间, 然后将segment的内容从ELF文
件中读入到这一内存区间, 并将[VirtAddr + FileSiz, VirtAddr + MemSiz)对
应的物理区间清零.*/

void naive_uload(PCB *pcb, const char *filename) {
  Log("in naive uloader");
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %d", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
