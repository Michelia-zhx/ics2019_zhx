#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

static inline uint32_t choose(uint32_t n) {
  return rand()%n;
}

static inline void gen(char str) {
  int l = choose(4);
  int r = choose(4);
  char s[1+l+r];
  for (int i=0;i<l;i++) {
    s[i] = ' '; 
  }
  s[l] = str;
  for (int j = l+1; j<l+r+1;j++) {
    s[j] = ' ';
  }
  s[r+l+1]='\0';
  strcat(buf, s);
}

static inline void gen_num() {
  char s[4];
  uint32_t n = choose(99);
  sprintf(s,"%u",n);
  strcat(buf, s);
}

static inline void gen_rand_op() {
  switch (choose(4)) {
    case 0: gen('+'); break;
    case 1: gen('-'); break;
    case 2: gen('*'); break;
    case 3: gen('/'); break;
  }
}

static inline void gen_rand_expr() {
  switch (choose(3)) {
    case 0: gen_num(); break;
    case 1: gen('('); gen_rand_expr(); gen(')'); break;
    default: gen_rand_expr(); gen_rand_op(); gen_rand_expr(); break;
  }
} 


int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
    
    memset(buf,'\0',65536);
  }
  return 0;
}
