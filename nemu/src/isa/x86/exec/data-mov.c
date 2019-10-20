#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //printf("here\n");
  rtl_push(&id_dest->val);
  rtlreg_t esp1 = cpu.esp;
  if (id_dest->type != OP_TYPE_IMM) operand_write(id_dest, &id_dest->val);
  //else if (id_dest->type == OP_TYPE_IMM) id_dest->imm = id_dest->val;
  //else assert(0);
  cpu.esp = esp1;
  
  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&id_dest->val);
  if (id_dest->type != OP_TYPE_IMM) operand_write(id_dest, &id_dest->val);
  //else if (id_dest->type == OP_TYPE_IMM) id_dest->imm = id_dest->val;
  //else assert(0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&reg_l(R_ESP), &reg_l(R_EBP));
  rtl_pop(&reg_l(R_EBP));

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decinfo.isa.is_operand_size_16) {
    bool msb = (reg_w(R_EAX) >> 15) & 1;
    if (msb) rtl_ori(&reg_l(R_EDX), &reg_l(R_EDX), 0xffff);
    else rtl_andi(&reg_l(R_EDX), &reg_l(R_EDX), 0xffff0000);
  }
  else {
    bool msb = (reg_l(R_EAX) >> 31) & 1;
    if (msb) rtl_li(&reg_l(R_EDX), 0xffffffff);
    else rtl_li(&reg_l(R_EDX), 0);
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decinfo.isa.is_operand_size_16) {
    TODO();
  }
  else {
    TODO();
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(&s0, &id_src->val, id_src->width);
  operand_write(id_dest, &s0);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
