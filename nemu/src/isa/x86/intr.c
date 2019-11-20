#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  // printf("in raise_intr.c\n");
  // printf("NO: %x, %d\n", NO, NO);
  uint32_t addr = cpu.idtr.base + NO*8; // 2

  uint32_t low = vaddr_read(addr, 2);
  uint32_t high = vaddr_read(addr+6, 2);
  uint32_t offset = (high << 16) | low; // 3

  rtl_push(&cpu.eflags.value);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);                  // 1

  rtl_j(offset);                        // 4
  cpu.eflags.IF = 0;
}

bool isa_query_intr(void) {
  return false;
}
