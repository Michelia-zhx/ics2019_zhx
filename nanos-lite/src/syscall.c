#include "common.h"
#include "syscall.h"
#include "proc.h"

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
    Log("SYS_write");
      if (a[1]==1 || a[1]==2){  // a[1] = fd
        char *addr = (char *)(a[2]);  // a[2] = (intptr_t)buf
        uintptr_t count = a[3];  // a[3] = count
        while (count){
          _putc(*addr);
          addr ++;
          count --;
        }
      }
      c->GPRx = a[3]; // On success, the number of bytes written  is  returned
      break;

    case SYS_brk:
      if (mm_brk((uintptr_t)a[1], (uintptr_t)a[2]))
        c->GPRx = 0;
      else c->GPRx = -1;
      break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return NULL;
}
