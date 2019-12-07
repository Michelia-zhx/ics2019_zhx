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

void isa_reg_display() {
  int i,j;
  for (i = 0; i <8; i++) {
    printf("%s:   %8x    %d \r\n", regsl[i], cpu.gpr[i]._32, cpu.gpr[i]._32);
  }
  for (i = 0; i <8; i++) {
    printf("%s:   %8x    %d  \r\n", regsw[i], cpu.gpr[i]._16, cpu.gpr[i]._16);
  }
  for (i = 0; i <8; i++) {
    for (j = 0; j <2; j++) {
      printf("%s:   %8x    %d  \r\n", regsb[i], cpu.gpr[i]._8[j], cpu.gpr[i]._8[j]);
    }
  }
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  uint32_t out=0;
  if(success) {
    for (int i=0; i<8; i++) {
      if(strcmp(s,regsl[i]) == 0) {
        out = cpu.gpr[i]._32;
        break; 
      }
      else if (strcmp(s,regsw[i]) == 0) {
        out = cpu.gpr[i]._16;
        break;
      }
      else if (strcmp(s,regsb[i]) == 0) {
        out = cpu.gpr[i]._8[0];
        break;
      }
    }
  }
  return out;
}
