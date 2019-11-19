#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //printf("here\n");
  rtl_push(&id_dest->val);
  if (id_dest->type != OP_TYPE_IMM) operand_write(id_dest, &id_dest->val);
  //else if (id_dest->type == OP_TYPE_IMM) id_dest->imm = id_dest->val;
  //else assert(0);
  
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
  if (decinfo.isa.is_operand_size_16)
  {
    rtl_li(&t0, reg_w(R_SP));
    rtl_li(&t1, reg_w(R_AX));
    rtl_push(&t1);
    rtl_li(&t1, reg_w(R_CX));
    rtl_push(&t1);
    rtl_li(&t1, reg_w(R_DX));
    rtl_push(&t1);
    rtl_li(&t1, reg_w(R_BX));
    rtl_push(&t1);
    rtl_push(&t0);
    rtl_li(&t1, reg_w(R_BP));
    rtl_push(&t1);
    rtl_li(&t1, reg_w(R_SI));
    rtl_push(&t1);
    rtl_li(&t1, reg_w(R_DI));
    rtl_push(&t1);
  }
  else{
    rtl_mv(&t0, &reg_l(R_ESP));
    rtl_push(&reg_l(R_EAX));
    rtl_push(&reg_l(R_ECX));
    rtl_push(&reg_l(R_EDX));
    rtl_push(&reg_l(R_EBX));
    rtl_push(&t0);
    rtl_push(&reg_l(R_EBP));
    rtl_push(&reg_l(R_ESI));
    rtl_push(&reg_l(R_EDI));
  }

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
    bool msb = (cpu.eax >> 7) & 1;
    if (msb) cpu.eax = cpu.eax | 0xff00;
    else cpu.eax = cpu.eax & 0x00ff;
  }
  else {
    bool msb = (cpu.eax >> 15) & 1;
    if (msb) cpu.eax = cpu.eax | 0xffff0000;
    else cpu.eax = cpu.eax & 0x0000ffff;
  }

  print_asm(decinfo.isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  //Log("%x", id_src->val);
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  if (id_src->width == 4){
    rtl_li(&s1, id_src->val & 0x0000ffff);
    id_src->width = 2;
  }
  else rtl_li(&s1, id_src->val);
  rtl_sext(&s0, &s1, id_src->width);
  //Log("%x", s0);
  operand_write(id_dest, &s0);
  // Log("%x", id_dest->val);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decinfo.isa.is_operand_size_16 ? 2 : 4;
  if(id_dest->width == 4) {
  	if(id_src->width == 1) rtl_li(&id_dest->val, id_src->val & 0x000000ff);
  	else rtl_li(&id_dest->val, id_src->val & 0x0000ffff);
  }
  else{
	  rtl_li(&id_dest->val,id_src->val & 0xffff00ff);
  }
  operand_write(id_dest, &id_dest->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  operand_write(id_dest, &id_src->addr);
  print_asm_template2(lea);
}
