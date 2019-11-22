#include "proc.h"
#include <elf.h>
#include <klib.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
# define Elf_off Elf64_Off
# define Elf_Half Elf64_Half
# define Elf_Word Elf64_Word
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
# define Elf_off Elf32_Off
# define Elf_Half Elf32_Half
# define Elf_Word Elf32_Word
#endif

size_t get_ramdisk_size();
size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

static uintptr_t loader(PCB *pcb, const char *filename) {
  printf("In loader.c\n");
  size_t ramdisk_size = get_ramdisk_size();
  printf("ramdisk_size:%d\n", ramdisk_size);

  Elf_Ehdr ehdr;
  size_t len = ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  printf("len %d\n", len);

  Elf_off phdr_offset = ehdr.e_phoff;
  printf("phdr_offset: %d\n", phdr_offset);
  Elf_Half phdr_size = ehdr.e_phentsize;
  printf("phdr_size: %d\n", phdr_size);
  Elf_Half phdr_num = ehdr.e_phnum;
  printf("phdr_num: %d\n", phdr_num);
  // assert(phdr_num == 3);

  for (int i=0; i<phdr_num; ++i) {
    Elf_Phdr phdr;
    printf("i: %d\n", i);
    len = ramdisk_read(&phdr, phdr_offset+i*phdr_size, phdr_size);
    printf("phdr.type: %d\n", phdr.p_type);
    Elf_off p_offset = phdr.p_offset;
    Elf_Word p_filesize = phdr.p_filesz;
    Elf_Word p_memsize = phdr.p_memsz;
    printf("p_offset: %d\n", p_offset);
    printf("p_filesize: %d\n", p_filesize);
    printf("p_memsize: %d\n", p_memsize);
    
    switch (phdr.p_type) {
      case PT_LOAD: {
        ramdisk_write((void *)(phdr.p_vaddr), phdr.p_offset, phdr.p_filesz);
        memset((void *)(phdr.p_vaddr+phdr.p_filesz), 0, (phdr.p_memsz-phdr.p_filesz));
        printf("hello\n");
        break;
      }
      default:
        break;
    }
  }
  printf("ehdr.e_entry: %d\n", ehdr.e_entry);
  return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
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
