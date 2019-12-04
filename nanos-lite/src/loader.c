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
  int fd = fs_open(filename, 0, 0);

  Elf_Ehdr ehdr;
  printf("size of ehdr:%d\n", sizeof(Elf_Ehdr));
  //ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));

  Elf_off ph_offset = ehdr.e_phoff;
  printf("ph_offset:%d\n", ph_offset);
  Elf_Half ph_size = ehdr.e_phentsize;
  printf("ph_size:%d\n", ph_size);
  Elf_Half ph_num = ehdr.e_phnum;
  printf("ph_num:%d\n", ph_num);

  for(int i = 0; i < ph_num; i++){   
    Elf_Phdr phdr;
    printf("offset+i*size:%d\n",ph_offset + i * ph_size);
    //ramdisk_read(&phdr, di_offset + ph_offset + i * ph_size, ph_size);
    printf("p_type:%d\n", phdr.p_type);
    printf("ptload:%d\n", PT_LOAD);
    printf("p_filesz:%d\n", phdr.p_filesz);
    fs_lseek(fd, ph_offset + i * ph_size, SEEK_SET);
    printf("ph_size:%d\n", ph_size);
    fs_read(fd, &phdr, ph_size);
    if(phdr.p_type == PT_LOAD){
        printf("phdr.p_vaddr:%d\n",phdr.p_vaddr);
        //ramdisk_write((const void *)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
        fs_lseek(fd, phdr.p_offset, SEEK_SET);
        fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
        memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
    }
  }
  printf("return:%d\n", ehdr.e_entry);
    
  fs_close(fd);
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
