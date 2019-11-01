#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#define bool int
#define true 1
#define false 0

extern uint32_t isa_reg_str2val(const char *, bool *);
enum {
  TK_NOTYPE = 256, TK_EQ = 257, TK_REG = 258, TK_HNUM = 259, TK_NUM = 260, TK_INEQ = 261, TK_AND = 262, DEREF = 263, NEG = 264,
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
  {"\\(", '('},         // left parenthesis
  {"\\)", ')'},         // right parenthesis
  {"/", '/'},           // divide
  {"\\*", '*'},         // multiply or DEREFERENCE
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus or NEG
  {"==", TK_EQ},        // equal
  {"\\$e?[abcds][xpilh]+", TK_REG},// register name
  {"0[xX][0-9a-fA-F]+", TK_HNUM}, // hexademical number
  {"[0-9]+", TK_NUM},   // demical number
  {"!=", TK_INEQ},      // inequal
  {"&&", TK_AND},       // and
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

//static Token tokens[65536] __attribute__((used)) = {};
static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;/*inform the compiler that a static variable is to be retained in the object file,even if it is unreferences */

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
        tokens[nr_token].type = rules[i].token_type;
        

        switch (rules[i].token_type) {
          case(256): break; // TK_NOTYPE
          case(257): case(261): case(262): case(263): case('('): case(')'): case('*'): case('/'): case('+'): case('-'):
            nr_token++; 
            break;    
          // TK_NOTYPE TK_EQ
          case(258):case(259): case(260): 
            strncpy(tokens[nr_token].str, substr_start, substr_len + 1); 
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++; 
            break;   
          // TK_REG TK_HNUM TK_NUM
          default: printf("No rule match!"); break;
        }
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

bool check_parentheses(int p, int q)
{  if(tokens[p].type == '(' && tokens[q].type == ')')
     { char stack[8] = {'\0'};
       int top = 0;
       for(int i = p+1; i < q; i++)
       { if (tokens[i].type == '(')
         { stack[top] = '(';
           top++;
         }
         else if(tokens[i].type == ')')
         { if(stack[top] == '(')
           { stack[top] = '\0';
             top--;
           }
           else if(stack[top] == '\0')
             return false;
         }
       }
       if(stack[top] == '\0' && top == 0)
         return true;
     }
   return false;
}

int find_op(Token tokens[], int p, int q)
{ int position = p;
  int re_position = q; // position to return
  int flag_AND = 0;
  int flag_EQ = 0;
  int flag_PLUS = 0;
  int flag_MUL = 0;
  int flag_DEF = 0;

  for(; position < q; position++)
    { int type = tokens[position].type;
      if(type == 258 || type == 259 || type == 260) // number...
        continue;
      else if (type == '(')
        { int j = position;
          for(; j <= q; j++)
            if(tokens[j].type == ')')
              {position = j;
               break;}
           //jump out of the brackets 
        }
      else if(type == 262) //&&
      { re_position = position;
        flag_AND = 1;
      }
      else if(type == 257 || type == 261) //!= ==
      { if(re_position < position && flag_AND == 1)
          continue;
        else
        { re_position = position;
          flag_EQ = 1;
        }
      }
      else if(type == '+' || type == '-')
        { if(re_position < position && (flag_AND == 1 || flag_EQ == 1))
            continue;
          else
          { re_position = position;
            flag_PLUS = 1;
          }
        }
      else if (type == '*' || type == '/')
        { if(re_position < position && (flag_AND == 1 || flag_EQ == 1 || flag_PLUS == 1))
            continue;
          else
            { re_position = position;
              flag_MUL = 1;
            }
        }
     else if (type == 263) // DEFER
       { if(re_position < position && (flag_AND == 1 || flag_EQ == 1 || flag_PLUS == 1 || flag_MUL == 1))
            continue;
          else
           { re_position = position;
             flag_DEF = 1;
           }    
       }  
    else if (type == 264) //NEG
      { if(re_position < position && (flag_AND == 1 || flag_EQ == 1 || flag_PLUS == 1 || flag_MUL == 1 || flag_DEF == 1))
          continue;
        else
          re_position = position;
      }  
    }
  return re_position;
}

uint32_t eval(int p, int q, bool *success) {
  if (p > q) {
    printf("Bad expression\n");
    *success = false;
    return 0;
  }
  else if (p == q) {
    return atoi(tokens[p].str);
  }
  else if (check_parentheses(p, q) == true) {
    return eval(p + 1, q - 1, success);
  } 
  else {
    int op = find_op(tokens, p, q);
    int op_type = tokens[op].type; 
    if(op_type == 263) // DEREF
       return atoi(tokens[op+1].str);
    if(op_type == 264) // NEG
       return atoi(tokens[op+1].str);
    uint32_t val1 = eval(p, op - 1, success);
    uint32_t val2 = eval(op + 1, q, success);
    switch (op_type) {
      case '+': return val1 + val2; break;
      case '-': return val1 - val2; break;
      case '*': return val1 * val2; break;
      case '/': 
        if(val2 == 0)
          { *success = false;
            return 0;}
        else
          return val1 / val2; 
        break;
      case(257): return val1 == val2; break; // ==
      case(261): return val1 != val2; break; // !=
      case(262): return val1 && val2; break; // &&
      default: *success = false; return 0;
    }
  }
}

uint32_t h2d(char str[]) //hexademical to demical
{  uint32_t value = 0;
   for(int i = 2; str[i] != '\0'; i++)
     {  if(str[i] <= 'f' && str[i] >= 'a')
          value = value*16 + str[i] -'a' + 10;
        else if(str[i] <= 'F' && str[i] >= 'A')
          value = value*16 + str[i] -'A' + 10;
        else
          value = value*16 + str[i] -'0';
     }
   return value;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i = 0; i < nr_token; i ++) 
  { if (tokens[i].type == 258)
     {  uint32_t val;
        char str_v[32];
        val = isa_reg_str2val(tokens[i].str, success);
        sprintf(str_v, "%d", val);
        strcpy(tokens[i].str, str_v);
        tokens[i].str[strlen(str_v)] = '\0';
     }
    if (tokens[i].type == 259)
     {  uint32_t value = h2d(tokens[i].str);
        char str_v[32];
        sprintf(str_v, "%d", value);
        strcpy(tokens[i].str, str_v);
        tokens[i].str[strlen(str_v)] = '\0';
     }
    if (tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == '+' || tokens[i - 1].type == '-' ||tokens[i - 1].type == '*' || tokens[i - 1].type == '/' || tokens[i - 1].type == '(' || tokens[i - 1].type == 257 || tokens[i - 1].type == 261 || tokens[i - 1].type == 262) ) 
      tokens[i].type = DEREF;
   if (tokens[i].type == 263)
    {  char addr[32];
       char str_v[32];
       strcpy(addr, tokens[i+1].str);
       paddr_t m;
       sscanf(addr, "%x", &m);
       uint32_t value;
       value = paddr_read(m, 4);
       sprintf(str_v, "%d", value);
       strcpy(tokens[i+1].str, str_v);
       tokens[i+1].str[strlen(str_v)] = '\0';
    }
   if(tokens[i].type == '-' && (i == 0 || tokens[i - 1].type == '+' || tokens[i - 1].type == '-' ||tokens[i - 1].type == '*' || tokens[i - 1].type == '/' || tokens[i - 1].type == '(' || tokens[i - 1].type == 257 || tokens[i - 1].type == 261 || tokens[i - 1].type == 262) ) 
      tokens[i].type = NEG;
   if(tokens[i].type == 264)
   { uint32_t value = atoi(tokens[i+1].str);
     value = 0 - value;
     char str_v[32];
     sprintf(str_v, "%d", value);
     strcpy(tokens[i+1].str, str_v);
     tokens[i+1].str[strlen(str_v)] = '\0';     
   }
  }
  uint32_t result;
  result = eval(0, nr_token - 1, success); 
  /* TODO: Insert codes to evaluate the expression. */
  
  return result;
}
