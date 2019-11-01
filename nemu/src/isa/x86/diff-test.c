#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if(ref_r->pc != cpu.pc)
  { printf("pc\n");
    return false;
  }
  for (int i = 0; i < 8; i++) {
    if(ref_r->gpr[i]._32 != cpu.gpr[i]._32)
    { printf("wrong:%d\n",i);
      return false;
    }
  }
  return true;
}

void isa_difftest_attach(void) {
}
