#include "common.h"
#include "syscall.h"
#include "proc.h"
#include "fs.h"

int mm_brk(uintptr_t brk, intptr_t increment);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_yield: 
      _yield();
      break;

    case SYS_exit:
      _halt(0);
      break;

    case SYS_write:
      
      // c->GPRx = a[3]; // On success, the number of bytes written  is  returned
      c->GPRx = fs_write(a[1], (void *)a[2], a[3]);
      break;

    case SYS_brk:
      c->GPRx = mm_brk((uintptr_t)a[1], (uintptr_t)a[2]);
      break;

    case(SYS_open):
    	c->GPRx = fs_open((const char*)a[1],a[2],a[3]);
    	break;

    case(SYS_read):
      Log("SYS_read");
      c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
      break;
    
    case(SYS_close):   
      Log("SYS_close");
      c->GPRx = fs_close(a[1]);
      break;
    
    case(SYS_lseek):
      Log("SYS_lseek");
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
