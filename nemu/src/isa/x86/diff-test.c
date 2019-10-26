#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  printf("ref_r.eax: %x, cpu.eax: %x\n", (*ref_r).eax, cpu.eax);
  
  if ((*ref_r).eax != cpu.eax) return false;
  if ((*ref_r).ecx != cpu.ecx) return false;
  if ((*ref_r).edx != cpu.edx) return false;
  printf("1\n");
  if ((*ref_r).ebx != cpu.ebx) return false;
  if ((*ref_r).esp != cpu.esp) return false;
  printf("7\n");
  printf("(*ref_r).ebp: %x, cpu.ebp: %x", (*ref_r).ebp, cpu.ebp);
  //if ((*ref_r).ebp != cpu.ebp) return false;
  printf("8\n");
  if ((*ref_r).esi != cpu.esi) return false;
  printf("(*ref_r).esi: %x, cpu.esi: %x", (*ref_r).esi, cpu.esi);
  printf("2\n");
  if ((*ref_r).edi != cpu.edi) return false;
  if ((*ref_r).gpr[0]._16 != cpu.gpr[0]._16) return false;
  if ((*ref_r).gpr[1]._16 != cpu.gpr[1]._16) return false;
  printf("3\n");
  if ((*ref_r).gpr[2]._16 != cpu.gpr[2]._16) return false;
  if ((*ref_r).gpr[3]._16 != cpu.gpr[3]._16) return false;
  if ((*ref_r).gpr[4]._16 != cpu.gpr[4]._16) return false;
  printf("4\n");
  //if ((*ref_r).gpr[5]._16 != cpu.gpr[5]._16) return false;
  if ((*ref_r).gpr[6]._16 != cpu.gpr[6]._16) return false;
  if ((*ref_r).gpr[7]._16 != cpu.gpr[7]._16) return false;
  if ((*ref_r).gpr[0]._8[0] != cpu.gpr[0]._8[0]) return false;
  printf("5\n");
  if ((*ref_r).gpr[0]._8[1] != cpu.gpr[0]._8[1]) return false;
  if ((*ref_r).gpr[1]._8[0] != cpu.gpr[1]._8[0]) return false;
  if ((*ref_r).gpr[1]._8[1] != cpu.gpr[1]._8[1]) return false;
  printf("6\n");
  if ((*ref_r).gpr[2]._8[0] != cpu.gpr[2]._8[0]) return false;
  //if ((*ref_r).gpr[2]._8[1] != cpu.gpr[2]._8[1]) return false;
  if ((*ref_r).gpr[3]._8[0] != cpu.gpr[3]._8[0]) return false;
  if ((*ref_r).gpr[3]._8[1] != cpu.gpr[3]._8[1]) return false;
  
  return false;
}

void isa_difftest_attach(void) {
}
