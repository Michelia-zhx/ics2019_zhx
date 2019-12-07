#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

uint32_t isa_reg_str2val(const char *s, bool *success);
uint32_t paddr_read();

enum {
  TK_NOTYPE = 256, TK_EQ, TK_UEQ, TK_SIXTEEN, TK_TEN, TK_REG, DEREF,

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
  {"\\+", '+'},         // plus point
  {"\\-", '-'},         // subtract
  {"\\*", '*'},         // multipy
  {"\\/", '/'},         // divide
  {"==", TK_EQ},         // equal
  {"!=", TK_UEQ},        // unequal
  {"0x[0-9a-f]+", TK_SIXTEEN},   //sixteen
  {"[0-9]+", TK_TEN},     // ten
  {"&&", '&'},          // and
  {"\\$[a-z]+", TK_REG},  // reg_name
  {"\\(", '('},         // left parenthrsis
  {"\\)", ')'}         // right parenthrsis
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

static Token tokens[65536] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (rules[i].token_type == TK_NOTYPE) continue;
        switch (rules[i].token_type) {
          case '+': tokens[nr_token].type = '+'; break;
          case '-': tokens[nr_token].type = '-'; break;
          case '*': tokens[nr_token].type = '*'; break;
          case '/': tokens[nr_token].type = '/'; break;
          case TK_EQ: tokens[nr_token].type = rules[i].token_type; memset(tokens[nr_token].str, '\0',1000); strncpy(tokens[nr_token].str,substr_start,substr_len); break;
          case TK_UEQ: tokens[nr_token].type = rules[i].token_type; memset(tokens[nr_token].str, '\0',1000); strncpy(tokens[nr_token].str,substr_start,substr_len); break;
          case TK_TEN: tokens[nr_token].type = rules[i].token_type; memset(tokens[nr_token].str, '\0',1000); strncpy(tokens[nr_token].str,substr_start,substr_len); break;
          case TK_SIXTEEN: tokens[nr_token].type = rules[i].token_type; memset(tokens[nr_token].str, '\0',1000); strncpy(tokens[nr_token].str,substr_start,substr_len); break;
          case TK_REG:tokens[nr_token].type = rules[i].token_type; memset(tokens[nr_token].str, '\0',1000); strncpy(tokens[nr_token].str,substr_start,substr_len); break;
          case '&': tokens[nr_token].type = '&'; break;
          case '(': tokens[nr_token].type = '('; break;
          case ')': tokens[nr_token].type = ')'; break;
          default: TODO();
        }
	nr_token ++;  
        break;
      }
    }
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q) {
  int left=0, right=0;
  if (tokens[p].type!='(' || tokens[q].type!=')') 
    return false;
  p = p+1;
  for(; p<q; p++){
    if (tokens[p].type == '(')
      left+=1;
    else if (tokens[p].type == ')')
      right+=1;
    if (right>left)
      return false; 
  }
  if (left!=right)
    return false;
  return true;  
}

uint32_t get_op(int p, int q) {
  uint32_t op=p;
  int count=0;//括号计数
  for (uint32_t i=p; i<q; i++) {
    if (tokens[i].type == '(')
      count++;
    else if (tokens[i].type == ')')
      count--;
    else if (count == 0) {
      if  (tokens[i].type == TK_EQ || tokens[i].type == TK_UEQ || tokens[i].type == '&') {
        op=i;
      }
      else if (tokens[i].type == '+' || tokens[i].type == '-') {
        op=i;   
      }
      else if (tokens[i].type == '*' || tokens[i].type == '/') {
        if (tokens[op].type != '+' && tokens[op].type != '-') {
          op=i;
        }
      }
    }
  }
  return op;
}


uint32_t eval(int p, int q) {
  if (p > q) {
    return false;
  }
  else if (p == q) {
    uint32_t out = 0;
    if (tokens[p].type == TK_TEN) {
      sscanf(tokens[p].str, "%d", &out);
      return out;
    }
    if (tokens[p].type == TK_SIXTEEN) {
      sscanf(tokens[p].str, "%x", &out);
      return out;
    }
    if (tokens[p].type == TK_REG) {
      for (int i = 0;i < 4;i ++) {
        tokens[p].str[i] = tokens[p].str[i+1];
      }
      bool success=true;
      out = isa_reg_str2val(tokens[p].str,&success);
      return out;
    }
  }
  else if (check_parentheses(p, q) == true) {
    return eval(p+1, q-1); 
  }
  else{
    uint32_t val1,val2;
    uint32_t op = get_op(p,q);
    if (op == p && tokens[p].type == DEREF) 
      return paddr_read(eval(p+1,q), 4);
    val1 = eval(p, op-1);
    val2 = eval(op+1, q);
    switch (tokens[op].type) {
      case '+': return val1 + val2;
      case '-': return val1 - val2;
      case '*': return val1 * val2;
      case '/': return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_UEQ: return val1 != val2;
      case '&':  return val1 && val2;
      default: assert(0);
    }
  }
  return 0;
} 


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  for (int i = 0; i< nr_token; i++) {
    if (tokens[i].type == '*' && (i == 0 || tokens[i-1].type == '+' || tokens[i-1].type == '-' || tokens[i-1].type == '*' || tokens[i-1].type == '/' || tokens[i].type == '(')) {
      tokens[i].type = DEREF;  
    }	
  }
  int p=0, q=nr_token-1;
  return eval(p,q);
}
