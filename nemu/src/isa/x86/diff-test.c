#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  printf("ref_r.eax: %x, cpu.eax: %x\n", (*ref_r).eax, cpu.eax);
  printf("ref_r.ecx: %x, cpu.ecx: %x\n", (*ref_r).ecx, cpu.ecx);
  printf("ref_r.edx: %x, cpu.edx: %x\n", (*ref_r).edx, cpu.edx);
  printf("ref_r.ebx: %x, cpu.ebx: %x\n", (*ref_r).ebx, cpu.ebx);
  printf("ref_r.esp: %x, cpu.esp: %x\n", (*ref_r).esp, cpu.esp);
  printf("ref_r.ebp: %x, cpu.ebp: %x\n", (*ref_r).ebp, cpu.ebp);
  printf("ref_r.esi: %x, cpu.esi: %x\n", (*ref_r).esi, cpu.esi);
  printf("ref_r.edi: %x, cpu.edi: %x\n", (*ref_r).edi, cpu.edi);
  /*
  if (ref_r->pc != cpu.pc) return false;
  for (int i=0; i<8; ++i){
    if (ref_r->gpr[i]._32 != cpu.gpr[i]._32) return false;
  }
  */
  if (ref_r->eax != cpu.eax) return false;
  if (ref_r->ecx != cpu.ecx) return false;
  if (ref_r->edx != cpu.edx) return false;
  if (ref_r->ebx != cpu.ebx) return false;
  if (ref_r->esp != cpu.esp) return false;
  if (ref_r->esi != cpu.esi) return false;
  if (ref_r->edi != cpu.edi) return false;
  return true;
}

void isa_difftest_attach(void) {
}
