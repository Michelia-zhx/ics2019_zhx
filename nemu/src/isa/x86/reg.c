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
  printf("Hello");
  printf("eax          %+20d \n", cpu.eax);
  printf("ecx          %+20d \n", cpu.ecx);
  printf("edx          %+20d \n", cpu.edx);
  printf("ebx          %+20d \n", cpu.ebx);
  printf("esp          %+20d \n", cpu.esp);
  printf("ebp          %+20d \n", cpu.ebp);
  printf("esi          %+20d \n", cpu.esi);
  printf("edi          %+20d \n", cpu.edi);
  /*printf("ax           %+20d \n", cpu.ax);
  printf("cx           %+20d \n", cpu.cx);
  printf("dx           %+20d \n", cpu.dx);
  printf("bx           %+20d \n", cpu.bx);
  printf("sp           %+20d \n", cpu.sp);
  printf("bp           %+20d \n", cpu.bp);
  printf("si           %+20d \n", cpu.si);
  printf("di           %+20d \n", cpu.di);
  printf("al           %+20d \n", cpu.al);
  printf("cl           %+20d \n", cpu.cl);
  printf("dl           %+20d \n", cpu.dl);
  printf("bl           %+20d \n", cpu.bl);
  printf("ah           %+20d \n", cpu.ah);
  printf("ch           %+20d \n", cpu.ch);
  printf("dh           %+20d \n", cpu.dh);
  printf("bh           %+20d \n", cpu.bh);*/
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
