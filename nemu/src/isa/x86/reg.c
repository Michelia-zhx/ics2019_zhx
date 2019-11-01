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
   printf("EAX   0x%x  %d\n",cpu.eax,cpu.eax);
   printf("ECX   0x%x  %d\n",cpu.ecx,cpu.ecx);
   printf("EDX   0x%x  %d\n",cpu.edx,cpu.edx);
   printf("EBX   0x%x  %d\n",cpu.ebx,cpu.ebx);
   printf("ESP   0x%x  %d\n",cpu.esp,cpu.esp);
   printf("EBP   0x%x  %d\n",cpu.ebp,cpu.ebp);
   printf("ESI   0x%x  %d\n",cpu.esi,cpu.esi);
   printf("EDI   0x%x  %d\n",cpu.edi,cpu.edi);
   printf("AX    0x%x  %d\n",cpu.gpr[0]._16, cpu.gpr[0]._16);
   printf("CX    0x%x  %d\n",cpu.gpr[1]._16, cpu.gpr[1]._16);
   printf("DX    0x%x  %d\n",cpu.gpr[2]._16, cpu.gpr[2]._16);
   printf("BX    0x%x  %d\n",cpu.gpr[3]._16, cpu.gpr[3]._16);
   printf("SP    0x%x  %d\n",cpu.gpr[4]._16, cpu.gpr[4]._16);
   printf("BP    0x%x  %d\n",cpu.gpr[5]._16, cpu.gpr[5]._16);
   printf("SI    0x%x  %d\n",cpu.gpr[6]._16, cpu.gpr[6]._16);
   printf("DI    0x%x  %d\n",cpu.gpr[7]._16, cpu.gpr[7]._16);
   printf("AL    0x%x  %d\n",cpu.gpr[0]._8[0], cpu.gpr[0]._8[0]);
   printf("CL    0x%x  %d\n",cpu.gpr[1]._8[0], cpu.gpr[0]._8[0]);
   printf("DL    0x%x  %d\n",cpu.gpr[2]._8[0], cpu.gpr[2]._8[0]);
   printf("BL    0x%x  %d\n",cpu.gpr[3]._8[0], cpu.gpr[3]._8[0]);
   printf("AH    0x%x  %d\n",cpu.gpr[0]._8[1], cpu.gpr[0]._8[1]);
   printf("CH    0x%x  %d\n",cpu.gpr[1]._8[1], cpu.gpr[1]._8[1]);
   printf("DH    0x%x  %d\n",cpu.gpr[2]._8[1], cpu.gpr[2]._8[1]);
   printf("BH    0x%x  %d\n",cpu.gpr[3]._8[1], cpu.gpr[3]._8[1]);
   return ;
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  for(int i = 0; i < 8; i++)
  { char expansion[5] = "$";
    char *d = strcat(expansion, regsl[i]);
    if(strcmp(s, d) == 0)
      { *success = true;
        return reg_l(i);}
    else
      continue;
  }
  for(int i = 0; i < 8; i++)
  { char expansion[4] = "$";
    char *d = strcat(expansion, regsw[i]);
    if(strcmp(s, d) == 0)
      { *success = true;
        return reg_w(i);}
    else
      continue;
  }
  for(int i = 0; i < 8; i++)
  { char expansion[4] = "$";
    char *d = strcat(expansion, regsb[i]);
    if(strcmp(s, d) == 0)
      { *success = true;
       return reg_b(i);}
    else
      continue;
  }
  *success = false;
  return 0;
}
