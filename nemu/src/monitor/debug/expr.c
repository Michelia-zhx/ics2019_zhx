#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <regex.h>
#include <string.h>

#define bool int
#define true 1
#define false 0
#define OVERFLOW -1
#define OK 1
#define ERROR 0
#define STACK_INIT_SIZE 100
#define STACKINCREMENT 10

typedef int SElemType;
typedef int Status;

typedef struct{
    SElemType *top;
    SElemType *base;
    int stacksize;
}Stack;

enum {
  TK_NOTYPE = 256, 
  TK_EQ,          //257
  TK_GPR,         //258
  TK_HEXADECIMAL, //259
  TK_DECIMAL,     //260
  TK_AND,         //261
  TK_OR,          //262
  TK_UEQ,         //263
  DEREF          //264
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  {" +", TK_NOTYPE},    // spaces
  {"0x[0-9a-f]+", TK_HEXADECIMAL},  // hexadecimal numbers
  {"\\$[e,a,b,c,d,s].*?[x,p,i,l,h]", TK_GPR},    //GPR
  {"\\+", '+'},         // plus
  {"\\-", '-'},         // minus
  {"\\*", '*'},         // times
  {"/", '/'},           //divide
  {"\\(", '('},         // left bracket
  {"\\)", ')'},         // right bracket
  {"\\[", '['},         // left bracket
  {"\\]", ']'},
  {"\\{", '{'},         // left bracket
  {"\\}", '}'},
  {"==", TK_EQ},            // equal
  {"!=", TK_UEQ},
  {"[0-9]+", TK_DECIMAL},   // decimal numbers
  {"&&", TK_AND},           // and
  {"\\|\\|", TK_OR}        // or
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

static Token tokens[512] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    //printf("position: %d\n", position);
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
        
        //printf("token_str: %s\n", token_str);

        switch (rules[i].token_type) {
          case '+': nr_token += 1; break;
          case '-': nr_token += 1; break;
          case '*': {
            if (nr_token == 0 || tokens[nr_token-1].type == '(' \
            || tokens[nr_token-1].type == '+' || tokens[nr_token-1].type == '-' \
            || tokens[nr_token-1].type == '*' || tokens[nr_token-1].type == '/' \
            || tokens[nr_token-1].type == 257 || tokens[nr_token-1].type == 263) {
              tokens[nr_token].type = DEREF;
            }
            nr_token += 1;
            break;
          }
          case '/': nr_token += 1; break;
          case '(': nr_token += 1; break;
          case ')': nr_token += 1; break;
          case '[': nr_token += 1; break;
          case ']': nr_token += 1; break;
          case '{': nr_token += 1; break;
          case '}': nr_token += 1; break;
          case 256: break;  //空格
          case 257: nr_token += 1; break;  // ==
          case 258: {              // gpr
            if(substr_len > 32){
              Log("Warning : the number is too long to restore. Only 32 nums in the front are saved.");
            }
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            //printf("The %d token is: %s\n", nr_token, tokens[nr_token].str);
            nr_token += 1; 
            break;
          }
          case 259: {                  //十六进制
            if(substr_len > 32){
              Log("Warning : the number is too long to restore. Only 32 nums in the front are saved.");
            }
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            //printf("The %d token is: %s\n", nr_token, tokens[nr_token].str);
            nr_token += 1; 
            break;
          }
          case 260: {                  //十进制
            if(substr_len > 32){
              Log("Warning : the number is too long to restore. Only 32 nums in the front are saved.");
            }
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            //printf("The %d token is: %s\n", nr_token, tokens[nr_token].str);
            nr_token += 1;
            break;
          }
          case 261: nr_token += 1; break;  //AND
          case 262: nr_token += 1; break;  //OR
          case 263: nr_token += 1; break;  //UEQ
          default: {
            printf("The token can be matched but whose type isn't here.\n");
          }
        }
        break;
      }
    }
    
    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  //printf("The last token is: %s\n", tokens[nr_token-1].str);
  //printf("The first type is: %d\n:", tokens[0].type);
  return true;
}

extern int check_parentheses(int p, int q, bool *success);
extern int find_dominated_op(int p, int q, bool *success);
extern uint32_t get_gpr(int p, bool *success);
extern int htoi(char s[]);

uint32_t eval(int p, int q, bool *success) {
  //printf("p:%d, q:%d\n", p, q);
  if (p > q) {
    Log("fatal error, the start of the sub-expression is bigger than its end.");
    return 0;
  }

  else if (p == q) {
    //printf("p:%d, q:%d\n", p, q);
    if (tokens[p].type == 259){
      int number = htoi(tokens[p].str);
      return number;
    }
    else if (tokens[p].type==260){
      int number = 0;
      for (int j=0; j<strlen(tokens[p].str); ++j){
        number = number*10 + (tokens[p].str[j]-'0');
        //printf("%d\n", number);
      }
      return number;
    }
    else if (tokens[p].type==258){
      uint32_t gpr_value = get_gpr(p, success);
      if (*success==true) return gpr_value;
      else return 0;
    }
    else {
      Log("Something wrong! The expression is illegal.");
      return 0;
    }
  }
  
  else{
    //Log("Starting checking parentheses!\n");
    if (check_parentheses(p, q, success) == 1) {
      //printf("p:%d, q:%d\n", p, q);
      /* The expression is surrounded by a matched pair of parentheses.
      * If that is the case, just throw away the parentheses.
      */
      return eval(p + 1, q - 1, success);
    }

    else {
      int op = find_dominated_op(p, q, success);
      
      if (tokens[op].type!=DEREF){
        uint32_t val1 = eval(p, op - 1, success);
        uint32_t val2 = eval(op + 1, q, success);

        switch (tokens[op].type) {
          case '+': return val1 + val2;
          case '-': return val1 - val2;
          case '*': return val1 * val2;
          case '/': {
            if (val2==0){
              *success = false;
              Log("ERROR: Division by zero");
              return 0;
            }
            else return val1 / val2;
          }
          case 257: return val1 == val2;
          case 261: return val1 && val2;
          case 262: return val1 || val2;
          case 263: return val1 != val2;
          default: {
            Log("Strange operation!");
            *success = false;
            return 0;
          }
        }
      }
      else{
        int val1 = eval(op+1, q, success);
			  return vaddr_read(val1,4);
      }
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (make_token(e)!=true) {
    Log("make token failed\n");
    *success = false;
    return 0;
  }

  int p = 0;
  int q = nr_token-1;
  return eval(p, q, success);
}


uint32_t get_gpr(int p, bool *success){
  if (strcmp(tokens[p].str, "$eax")==0) return cpu.eax;
  else if (strcmp(tokens[p].str, "$ecx")==0) return cpu.ecx;
  else if (strcmp(tokens[p].str, "$edx")==0) return cpu.edx;
  else if (strcmp(tokens[p].str, "$ebx")==0) return cpu.ebx;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.esp;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.ebp;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.esi;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.edi;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[0]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[1]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[2]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[3]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[4]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[5]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[6]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[7]._16;
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[0]._8[0];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[0]._8[1];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[1]._8[0];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[1]._8[1];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[2]._8[0];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[2]._8[1];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[3]._8[0];
  else if (strcmp(tokens[p].str, "$eax")==0) return cpu.gpr[3]._8[1];
  else {
    Log("No such register");
    *success = false;
    return 0;
  }
}


Status InitStack(Stack *S){
    (*S).base=(SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
    if(!(*S).base) return(OVERFLOW);
    else {
        (*S).top=(*S).base;
        (*S).stacksize=STACK_INIT_SIZE;
        return OK;
    }
}

Status DestroyStack(Stack *S){    
    free((*S).base);
    (*S).base=NULL;
    (*S).top=NULL;
    (*S).stacksize=0;
    return OK;
}

Status StackEmpty(Stack *S){
    if((*S).top==(*S).base) return OK;
    else return ERROR;
}

Status Push(Stack *S, SElemType e){
    if((*S).top-(*S).base >= (*S).stacksize){
        (*S).base = (SElemType *)realloc((*S).base, ((*S).stacksize+STACKINCREMENT)*sizeof(SElemType));
        if(!(*S).base) exit(OVERFLOW);        //储存分配失败
        (*S).top = (*S).base + (*S).stacksize;
        (*S).stacksize += STACKINCREMENT;
    }
    (*S).top++;
    *(*S).top = e;
    
    return OK;
}

Status Pop(Stack *S, SElemType *e){
    if(S->top == S->base) return ERROR;
    else {
        //printf("top:%d\n",*(S->top));
        *e = *(S->top);
        S->top -= 1;
        return OK;
    }
}

 

bool check_parentheses(int p, int q, bool *success){
    Stack S;
    //printf("hi!");
    int e,temp[65536];
    int a = 0;
    int *m = &a;
    //printf("hi!!");
    int i=p;
    int flag=1;

    for (i=p; i<=q; ++i)  temp[i] = tokens[i].type;
    InitStack(&S);
    if (temp[p]!='('||temp[q]!=')'){
        //printf("false, the whole expression is not surrounded by a matched pair of parentheses");
        return false;
    }
    //printf("hi!!!");
    i=p;
    e=temp[i];

    while(i<=q && flag){
      //printf("%d\n", e);
        switch(e){
            case '(':   case '[':   case '{': {
                //printf("(\n");
                if (!StackEmpty(&S)||i==p){
                    Push(&S, e);
                }
                else{
                    //printf("false, the leftmost '(' and the rightmost ')' are not matched");
                    flag = 0;
                }
                i++;
                e = temp[i];
                break; //左括号入栈
            }
            case ')':{
                //printf(")\n");
                if (!StackEmpty(&S)){
                    Pop(&S, m);
                    //printf("m:%d", *m);
                    if (*m!='('){
                      flag=0;
                      *success = false;
                    }
                }
                else {
                    Log("false, bad expression");
                    flag=0;
                    *success = false;
                }
                i++;
                e=temp[i];
                break;
            }
            case ']':{
                if(!StackEmpty(&S)){
                    Pop(&S, m);
                    if(*m!='['){
                      flag=0; 
                      *success = false;
                    }
                }
                else {
                    Log("false, bad expression");
                    flag=0;
                    *success = false;
                }
                i++;
                e=temp[i];
                break;
            }
            case '}':{
                if(!StackEmpty(&S)){
                    Pop(&S, m);
                    if(*m!='{'){
                      flag=0;
                      *success = false;
                    }
                }
                else {
                    printf("false, bad expression");
                    flag=0;
                    *success = false;
                }
                i++;
                e=temp[i];
                break;
            }
            default:{
                i++;
                e=temp[i];
                break;
            }
        }
    }
    if(!StackEmpty(&S)) flag=0;

    DestroyStack(&S);
    //printf("%d\n", flag);
    if(flag==1) return true;
    else return false;
}


int find_dominated_op(int p, int q, bool *success){
  int op = p;
  int position = p;
  int op_priority = 100;
  int cur_priority = 100;
  int num_left_parentheses = 0;
  while (position<=q){
    switch (tokens[position].type)
    {
      case 262:{
        if (num_left_parentheses==0){
          cur_priority = 0;
          if (cur_priority <= op_priority){
            op_priority = cur_priority;
            op = position;
          }
        }
        position += 1;
        break;
      }

      case 261:{
        if (num_left_parentheses==0){
          cur_priority = 1;
          if (cur_priority <= op_priority){
            op_priority = cur_priority;
            op = position;
          }
        }
        position += 1;
        break;
      }


      case 257: case 263: {
        if (num_left_parentheses==0){
          cur_priority = 2;
          if (cur_priority <= op_priority){
            op_priority = cur_priority;
            op = position;
          }
        }
        position += 1;
        break;
      }
      
      case '+': case '-':{
        if (num_left_parentheses==0){
          cur_priority = 3;
          if (cur_priority <= op_priority){
            op_priority = cur_priority;
            op = position;
          }
        }
        position += 1;
        break;
      } 

      case '*': case '/': {
        if (num_left_parentheses==0){
          cur_priority = 4;
          if (cur_priority <= op_priority){
            op_priority = cur_priority;
            op = position;
          }
        }
        position += 1;
        break;
      }

      case 264: {
        if (num_left_parentheses==0){
          cur_priority = 5;
          if (cur_priority <= op_priority){
            op_priority = cur_priority;
            op = position;
          }
        }
        position += 1;
        break;
      }

      case '(':{
        num_left_parentheses += 1;
        position += 1;
        break;
      }

      case ')':{
        num_left_parentheses -= 1;
        position += 1;
        break;
      }

      default:{
        position += 1;
        break;
      }
    }
  }
  return op;
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