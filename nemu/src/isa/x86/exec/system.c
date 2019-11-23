#include "cpu/exec.h"

make_EHelper(lidt) {
  // printf("in lidt\n");
  if (decinfo.isa.is_operand_size_16){
    cpu.idtr.base = vaddr_read(id_dest->addr+2, 3);
    cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  }
  else{
    cpu.idtr.base = vaddr_read(id_dest->addr+2, 4);
    cpu.idtr.limit = vaddr_read(id_dest->addr, 2);
  }

  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

extern void raise_intr(uint32_t NO, vaddr_t ret_addr);

make_EHelper(int) {
  // printf("in int\n");
  // printf("id_dest->val: %x, %d\n", id_dest->val, id_dest->val);
  raise_intr(id_dest->val, cpu.pc+2);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&t0);
  rtl_pop(&cpu.cs);
  rtl_pop(&cpu.eflags.value);
  rtl_j(t0);

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  if (id_src->width == 4) rtl_li(&id_dest->val, pio_read_l((ioaddr_t)id_src->val));
  else if (id_src->width == 2) rtl_li(&id_dest->val, pio_read_w((ioaddr_t)id_src->val));
  else if (id_src->width == 1) rtl_li(&id_dest->val, pio_read_b((ioaddr_t)id_src->val));
  else {
    printf("should not reach here.");
    assert(0);
  }
  operand_write(id_dest, &id_dest->val);

  print_asm_template2(in);
}

make_EHelper(out) {
  /*
  if (id_src->width == 4) pio_write_l((ioaddr_t)id_dest->val, id_src->val);
  else if (id_src->width == 2) pio_write_w((ioaddr_t)id_dest->val, id_src->val);
  else if (id_src->width == 1) pio_write_b((ioaddr_t)id_dest->val, id_src->val);
  else {
    printf("should not reach here.");
    assert(0);
  }
  */
  pio_write_b((ioaddr_t)id_dest->val,id_src->val);
  print_asm_template2(out);
}
