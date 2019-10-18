#include "cpu/exec.h"

//data-mov.c
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(lea);
make_EHelper(movzx);
make_EHelper(leave);

make_EHelper(operand_size);

//control.c
make_EHelper(call);
make_EHelper(ret);

//arith.c
make_EHelper(add);
make_EHelper(sub);
make_EHelper(cmp);

//logic.c
make_EHelper(test);
make_EHelper(and);
make_EHelper(xor);
make_EHelper(setcc);

//special.c
make_EHelper(nop);
make_EHelper(inv);
make_EHelper(nemu_trap);