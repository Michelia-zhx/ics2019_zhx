#include "monitor/expr.h"
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  /*
    bool *success = (bool*)malloc(sizeof(bool));
    FILE *f = fopen("./tools/gen-expr/input", "r");
    char exp[10000];
    int correct;
    for (int i=0; i<10; ++i){
      exp[0] = '\0';
      if (fscanf(f, "%u %s", &correct, exp)!=1) continue;
      int result = expr(exp, success);
      if (*success==true) printf("%u %u", correct, result);
    }
    free(success);
  */
 
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
