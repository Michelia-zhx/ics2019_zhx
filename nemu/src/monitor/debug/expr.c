#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <string.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_GPR, TK_HEXADECIMAL, TK_DECIMAL

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},            // minus
  {"\\*", '*'},         // times
  {"/", '/'},           //divide
  {"\\(", '('},           // left bracket
  {"\\)", ')'},           // right bracket
  {"==", TK_EQ},        // equal
  {"\\$[e,a,b,c,d,s].*?[x,p,i,l,h]", TK_GPR},    //GPR
  {"0x[0-9]+", TK_HEXADECIMAL},  // hexadecimal numbers
  {"[0-9]+", TK_DECIMAL}    // decimal numbers
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    char token_str[32];
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        //printf("%s\n", substr_start);
        int substr_len = pmatch.rm_eo;
        //printf("substr_len:%d \n", substr_len);
        //printf("token type: %d \n", rules[i].token_type);

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        tokens[nr_token].type = rules[i].token_type;
        
        for (int j=0; j<substr_len; ++j){
          token_str[j] = substr_start[j];
          token_str[substr_len] = '\0';
        }
        nr_token += 1;
        //printf("token_str: %s\n", token_str);

        switch (rules[i].token_type) {
          case '+': break;
          case '-': break;
          case '*': break;
          case '/': break;
          case '(': break;
          case ')': break;
          case 256: nr_token -= 1; break;
          case 257: break;
          case 258: {
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            printf("The token is: %s\n", tokens[nr_token].str);
            break;
          }
          case 259: {
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            printf("The token is: %s\n", tokens[nr_token].str);
            break;
          }
          case 260: {
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            printf("The token is: %s\n", token_str);
            break;
          }
          default: {
            printf("The token can be matched but whose type isn't here.\n");
          };
        }
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  printf("The first token is: %s\n", tokens[0].str);
  return true;
}

uint32_t eval(int p, int q) {
  if (p > q) {
    printf("fatal error, the start of the sub-expression is bigger than its end.");
    return 0;
  }
  else if (p == q) {
    printf("token type: %d, token str: %s\n", tokens[p].type, tokens[p].str);
    if (tokens[p].type == 259){
      int number = 0;
      for (int j=2; j<strlen(tokens[p].str); ++j){
        number = number*16 + tokens[p].str[j];
      }
      return number;
    }
    else if (tokens[p].type==260){
      int number = 0;
      for (int j=0; j<strlen(tokens[p].str); ++j){
        number = number*10 + tokens[p].str[j];
      }
      return number;
    }
    else {
      printf("Something wrong! The expression is illegal.");
      return 0;
    }
  }
  //else if (check_parentheses(p, q) == true) {
      /* The expression is surrounded by a matched pair of parentheses.
      * If that is the case, just throw away the parentheses.
      */
  //    return eval(p + 1, q - 1);
  //}
  //else {
      /* We should do more things here. */
  //}
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;

    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  printf("Hello, %d %s\n", tokens[0].type, tokens[0].str);
  return eval(0, nr_token-1);
}
