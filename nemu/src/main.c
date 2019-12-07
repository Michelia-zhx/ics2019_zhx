#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<monitor/expr.h>


int init_monitor(int, char *[]);
void ui_mainloop(int);
uint32_t expr(char *e, bool *success);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

 /* char *buf = (char*)malloc(65536*sizeof(char));
  int suc=0;
  FILE *fp = fopen("/home/cx/ics2019/nemu/tools/gen-expr/input","r");
  while(fgets(buf,65536,fp)!=NULL) {
    char *arg1 = strtok(buf, " ");
    char *arg2 = strtok(NULL, "\n");
    bool success = true;
    int result=expr(arg2,&success);
    if (result==atoi(arg1))
      suc++;
    memset(buf, 0, 65536);
  }
  fclose(fp);
  float acc = suc/100.0;
  printf("%f\n",acc);*/
  
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  
  return 0;
}
