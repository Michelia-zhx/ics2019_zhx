#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  /*va_list ap;
  int val, val1;
  int re = 0;
  char *s;
  va_start(ap, fmt);
  while(*fmt){
    char s = *fmt;
    if(strcmp(s, '%') == 0){
      fmt++;
      switch(fmt)
        case'd':
          val = va_arg(ap, int);
          val1 = val;
          int count = 0;
          while(val1 != 0){
            val1 /= 10;
            count++;
          }
          re += count;
          char v = atoi(val);
          printf(v);
          break;
        case's':
          s = va_arg(ap, char*);
          re += strlen(s);
          printf(s);
          break;
        default: return 0;break;
     }
    fmt++;
  }
  va_end(ap);
  return re;
  */
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int re = 0;
  char *str;  
  while(*fmt){ 
    if(*fmt == '%'){
      fmt++;
      int val,val1;
      switch(*fmt){
        case 'd':
          val = va_arg(ap, int);
          val1 = val;
          int count = 0;
          while(val1){
            val1 /= 10;
            count ++;
          }
          val1 = val;
          re += count;
          char num[100];
          int count1 = count;
          while(count){
            int t = val1 % 10;
            num[count - 1] = t + '0';
            val1 /= 10;
            count--;
          }
          for(int i = 0; i < count1; i++){
            *out = num[i];
            //_putc(*out);
            out++;
          }
          break;
        case 's':
          str = va_arg(ap, char *);
          int len = strlen(str);
          re += len; 
          for(int i = 0; i < len; i++){
            *out = *str;
            //_putc(*out);
            out++;
            str++;
          }
          break;
        default://printf("More to be added...\n"); 
          break;
        }  
    fmt++;
    }
    else {
      *out = *fmt;
      //_putc(*out);
      out++;
      fmt++;
      re++;
    }
  }
  *out = '\0';
  return re;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int re = vsprintf(out, fmt, ap);
  va_end(ap);
  return re;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
