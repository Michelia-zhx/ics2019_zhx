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
typedef int  Status;

typedef struct{
    SElemType *top;
    SElemType *base;
    int stacksize;
}Stack;

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
  {"\\[", '['},           // left bracket
  {"\\]", ']'},
  {"\\{", '{'},           // left bracket
  {"\\}", '}'},
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
    printf("position: %d\n", position);
    /* Try all rules one by one. */
    char token_str[32];

    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        //printf("%s\n", substr_start);
        int substr_len = pmatch.rm_eo;
        printf("substr_len:%d \n", substr_len);
        printf("token type: %d \n", rules[i].token_type);

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
          case '*': nr_token += 1; break;
          case '/': nr_token += 1; break;
          case '(': nr_token += 1; break;
          case ')': nr_token += 1; break;
          case '[': nr_token += 1; break;
          case ']': nr_token += 1; break;
          case '{': nr_token += 1; break;
          case '}': nr_token += 1; break;
          case 256: nr_token += 1; break;
          case 257: nr_token += 1; break;
          case 258: {
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            printf("The %d token is: %s\n", nr_token, tokens[nr_token].str);
            nr_token += 1; 
            break;
          }
          case 259: {
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            printf("The %d token is: %s\n", nr_token, tokens[nr_token].str);
            nr_token += 1; 
            break;
          }
          case 260: {
            strncpy(tokens[nr_token].str, token_str, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            printf("The %d token is: %s\n", nr_token, tokens[nr_token].str);
            nr_token += 1;
            break;
          }
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
  printf("The first type is: %d\n:", tokens[0].type);
  return true;
}

extern int check_parentheses(int p, int q);

uint32_t eval(int p, int q) {
  if (p > q) {
    printf("fatal error, the start of the sub-expression is bigger than its end.");
    return 0;
  }

  else if (p == q) {
    if (tokens[p].type == 259){
      int number = 0;
      for (int j=2; j<strlen(tokens[p].str); ++j){
        number = number*16 + (tokens[p].str[j]-'0');
      }
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
    //else if (tokens[p].type==258){
      //return get_gpr(tokens[p].str);
    //}
    else {
      printf("Something wrong! The expression is illegal.");
      return 0;
    }
  }
  
  else{
    printf("Starting checking parentheses!\n");
    if (check_parentheses(p, q) == 1) {
      /* The expression is surrounded by a matched pair of parentheses.
      * If that is the case, just throw away the parentheses.
      */
      return eval(p + 1, q - 1);
    }

    else {
        /* We should do more things here. */
        TODO();
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (make_token(e)!=true) {
    printf("make token failed\n");
    *success = false;

    return 0;
  }

  printf("make token success\n");
  int p = 0;
  int q = nr_token-1;

  /* TODO: Insert codes to evaluate the expression. */
  //printf("Hello, %d %s\n", tokens[0].type, tokens[0].str);
  return eval(p, q);
}









































Status InitStack(Stack *S){
    (*S).base=(SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));     //开辟空间
    if(!(*S).base) return(OVERFLOW);      //存储分配失败
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
    *(*S).top = e;
    (*S).top++;
    return OK;
}

Status Pop(Stack *S, SElemType *e){
    if(S->top == S->base) return ERROR;
    else {
        e = S->top;
        S->top -= 1;
        return OK;
    }
}

 

bool check_parentheses(int p, int q){
    Stack S;
    //printf("hi!");
    int e,temp[32];
    int a = 0;
    int *m = &a;
    //printf("hi!!");
    int i=p;
    int flag=1;

    for (i=p; i<=q; ++i)  temp[i] = tokens[i].type;
    InitStack(&S);
    if (temp[p]!=40||temp[q]!=41){
        printf("false, the whole expression is not surrounded by a matched pair of parentheses");
        return false;
    }
    //printf("hi!!!");
    i=p;
    e=temp[i];

    while(i<=q && flag){
      //printf("%d\n", e);
        switch(e){
            case '(':   case '[':   case '{': {
                if (!StackEmpty(&S)||i==p){
                    Push(&S, e);
                }
                else{
                    printf("false, the leftmost '(' and the rightmost ')' are not matched");
                    flag = 0;
                }
                i++;
                e = temp[i];
                break; //左括号入栈
            }
            case ')':{
                if (!StackEmpty(&S)){
                    Pop(&S, m);                  //右括号 匹配的话 出栈
                    if (e!='(') flag=0;
                }
                else if (i!=q) {
                    printf("false, bad expression");
                    flag=0;
                }
                i++;
                e=temp[i];
                break;
            }
            case ']':{
                if(!StackEmpty(&S)){
                    Pop(&S, m);
                    if(e!='[') flag=0;
                }
                else if (i!=q) {
                    printf("false, bad expression");
                    flag=0;
                }
                i++;
                e=temp[i];
                break;
            }
            case '}':{
                if(!StackEmpty(&S)){
                    Pop(&S, m);
                    if(e!='{')   flag=0;
                }
                else if (i!=q) {
                    printf("false, bad expression");
                    flag=0;
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

    if(flag==1) return true;
    else  return false;
}