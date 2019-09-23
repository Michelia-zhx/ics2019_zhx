#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>

void cpu_exec(uint64_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static int cmd_p(char *args);

static int cmd_x_N(char *args);

static int cmd_w(char *args);

static int cmd_d(char *args);


static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step through the program to run N orders and then pause, default N=1", cmd_si},
  { "info", "Display the state of the program", cmd_info},
  { "p", "Calculate the value of expression EXPR", cmd_p},
  { "x", "Calculate the value of expression EXPR and set as the starting memory address, print successive N 4 bytes in hexadecimal form", cmd_x_N},
  { "w", "When the value of EXPR changes, pause the program", cmd_w},
  { "d", "Delete the watchpoint of index N", cmd_d}

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;
  printf("%s", arg);

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_si(char *args){
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i=0;
  if (arg == NULL) i = 1;
  else i = atoi(arg);
  if (i==0) {
    Log("Invalid Input");
    i = 1;
  }
  for (int j=1; j<=i; ++j){
    cpu_exec(1);
  }
  return 0;
}//单步执行

extern void isa_reg_display();

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");
  if (arg==NULL){
    printf("For example: 'info r'--display the state of register; 'info w'--display the state of watchpoint.");
    return 0;
  }
  if (*arg=='w'){
  }
  if (*arg=='r'){
    isa_reg_display();
  }
  return 0;
}//打印寄存器

static int cmd_p(char *args){
  //char *arg = strtok(NULL, " ");
  bool suc = true;
  bool *success = &suc;
  int result = expr(args, success);
  if (*success==true){
    printf("$ %d\n", result);
    return 0;
  }
  else return 0;
}

extern uint32_t paddr_read(paddr_t addr, int len);
extern int htoi(char s[]);

static int cmd_x_N(char *args){
  char *arg = strtok(NULL, " ");
  int n = atoi(arg);
  if (n<=0){
    Log("Invalid Input");
    return -1;
  }
  arg = strtok(NULL, " ");
  char addr[15];
  strcpy(addr, arg);
  paddr_t address = htoi(addr);
  printf("the address is:%x\n", address);
  paddr_t four = 0x4;
  for (int i=0; i<n; ++i){
    if (i%4==0){
      printf("0x%-14x:", address);
    }
    printf("0x%-14x", paddr_read(address, 4));
    address = address + four;
    if ((i+1)%4==0){
      printf("\n");
    }
  }
  printf("\n");
  return 0;
}//扫描内存

static int cmd_w(char *args){
  return 0;
}

static int cmd_d(char *args){
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

int htoi(char s[])
{
	paddr_t n = 0;
	paddr_t i = 0;
	while (s[i] != '\0' && s[i] != '\n') {
		if (s[i] == '0') {
			if (s[i+1] == 'x' || s[i+1] == 'X')
                            i+=2;
		}
		if (s[i] >= '0' && s[i] <= '9') {
			n = n * 16 + (s[i] - '0');
		} else if (s[i] >= 'a' && s[i] <= 'f') {
			n = n * 16 + (s[i] - 'a') + 10;
		} else if (s[i] >= 'A' && s[i] <= 'F') {
			n = n * 16 + (s[i] - 'A') + 10;
		} else
			return -1;
		++i;

	}
	return n;
}
