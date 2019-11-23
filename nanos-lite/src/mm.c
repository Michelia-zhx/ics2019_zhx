#include "memory.h"
#include "proc.h"

static void *pf = NULL;
extern PCB *current;

int _map(_AddressSpace *as, void *va, void *pa, int prot);

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
  /*
  uintptr_t cur_brk = brk + increment;
  if (cur_brk > current->max_brk){
    int new_pg_num = (cur_brk - current->max_brk)/PGSIZE + 1;
    for (int i=0; i<new_pg_num; i ++){
      void *newpage = new_page(1);
      _map(&(current->as), (void *)(current->max_brk), newpage, 1);
      current->max_brk += PGSIZE;
    }
  }
  */
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
