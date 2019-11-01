#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "string.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>


void cpu_exec(uint64_t n);
uint32_t paddr_read(paddr_t addr, int len);
WP *new_wp(char *);
void free_wp(char *);
void wp_display();

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
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_test_expr();
static int cmd_w(char *args);
static int cmd_d(char *args);

extern void isa_reg_display(void);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Let the program step through N instructions and then suspend execution", cmd_si },
  { "info", "Print register status", cmd_info },
  { "x", "Scan memory", cmd_x},
  { "p", "Evaluation of expression", cmd_p},
  { "t", "Test expr.c", cmd_test_expr},
  { "w", "Set watchpoints", cmd_w},
  { "d", "Delete watchpoints", cmd_d},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

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

static int cmd_si(char *args)
{   char *arg = strtok(NULL, " ");
    int n = 0;

    if (arg == NULL) 
    {
      cpu_exec(1);
      return 0;
    }
    else {
     for (int j = 0; j < strlen(arg); j++)
       n = n*10 + arg[j] -'0';
     for (int i = 0;i < n; i++)
       cpu_exec(1);
     }
   return 0;
}

static int cmd_info(char *args)
{   char *arg = strtok(NULL, " ");
     if (strcmp(arg, "r") == 0)
       isa_reg_display();
     else if (strcmp(arg, "w") == 0)
       wp_display();
     else printf("Unknown command '%s'\n", arg);
     return 0;
}

static int cmd_w(char *args)
{ char *expr = strtok(NULL, " ");
  WP *n_wp __attribute__((unused))= new_wp(expr);
  return 0;
}

static int cmd_d(char *args)
{ char *arg = strtok(NULL, " ");
  free_wp(arg);
  return 0;
}

static int cmd_x(char *args)
{   char *arg = strtok(NULL, " ");
    int n = 0;

    for (int j = 0; j < strlen(arg); j++)
       n = n*10 + arg[j] -'0';

    char *addr = strtok(NULL, " ");
    paddr_t m;
    sscanf(addr, "%x", &m);
    for (int i = 0; i < n; i++, m++)
       printf("%u\n", paddr_read(m, 4));
    return 0;
}

static int cmd_p(char *args)
{  /*char *arg = strtok(NULL, " ");*/
   bool s = 1;
   bool *success = &s;
   uint32_t result;
   result = expr(args, success);
   if(*success == true)
     printf("result:%d\n",result);
   else
     printf("Error!\n");
   return 0;
}

 //test expr.c
static int cmd_test_expr()
{ 
  FILE *f;
  char buf[65536];
  char *result;
  char *exp;
  bool s = 0;
  bool *success = &s;
  int count = 0;
  char *return_ __attribute__((unused)) ;

  f = fopen("/home/yyc/ics2019/nemu/tools/gen-expr/input", "r");
  return_ = fgets(buf, 65536, f);
  for(int i = 0; i < 100; i++)
  {  
     int len_1 = strlen(buf);
     buf[len_1-1] = '\0';
     result = strtok(buf, " ");
     exp = strtok(NULL,  " ");
     uint32_t a = expr(exp, success);
     if(a == atol(result))
        { count++; //right number
          printf("Success!");
        }
     else
        printf("5555");
     return_ = fgets(buf, 65536, f);
   }
  printf("right number is:%d\n", count);
  fclose(f);
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
