#include "nemu.h"
#include <stdlib.h>
#include <time.h>

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display(){
  printf("eax    %-20x      %-20d \n", cpu.eax, cpu.eax);
  printf("ecx    %-20x      %-20d \n", cpu.ecx, cpu.ecx);
  printf("edx    %-20x      %-20d \n", cpu.edx, cpu.edx);
  printf("ebx    %-20x      %-20d \n", cpu.ebx, cpu.ebx);
  printf("esp    %-20x      %-20d \n", cpu.esp, cpu.esp);
  printf("ebp    %-20x      %-20d \n", cpu.ebp, cpu.ebp);
  printf("esi    %-20x      %-20d \n", cpu.esi, cpu.esi);
  printf("edi    %-20x      %-20d \n", cpu.edi, cpu.edi);
  printf("ax     %-20x      %+20d \n", cpu.gpr[0]._16, cpu.gpr[0]._16);
  printf("cx     %-20x      %+20d \n", cpu.gpr[1]._16, cpu.gpr[1]._16);
  printf("dx     %-20x      %+20d \n", cpu.gpr[2]._16, cpu.gpr[2]._16);
  printf("bx     %-20x      %+20d \n", cpu.gpr[3]._16, cpu.gpr[3]._16);
  printf("sp     %-20x      %+20d \n", cpu.gpr[4]._16, cpu.gpr[4]._16);
  printf("bp     %-20x      %+20d \n", cpu.gpr[5]._16, cpu.gpr[5]._16);
  printf("si     %-20x      %+20d \n", cpu.gpr[6]._16, cpu.gpr[6]._16);
  printf("di     %-20x      %+20d \n", cpu.gpr[7]._16, cpu.gpr[7]._16);
  printf("al     %-20x      %+20d \n", cpu.gpr[0]._8[0], cpu.gpr[0]._8[0]);
  printf("cl     %-20x      %+20d \n", cpu.gpr[0]._8[1], cpu.gpr[0]._8[1]);
  printf("dl     %-20x      %+20d \n", cpu.gpr[1]._8[0], cpu.gpr[1]._8[0]);
  printf("bl     %-20x      %+20d \n", cpu.gpr[1]._8[1], cpu.gpr[1]._8[1]);
  printf("ah     %-20x      %+20d \n", cpu.gpr[2]._8[0], cpu.gpr[2]._8[0]);
  printf("ch     %-20x      %+20d \n", cpu.gpr[2]._8[1], cpu.gpr[2]._8[1]);
  printf("dh     %-20x      %+20d \n", cpu.gpr[3]._8[0], cpu.gpr[3]._8[0]);
  printf("bh     %-20x      %+20d \n", cpu.gpr[3]._8[1], cpu.gpr[3]._8[1]);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
