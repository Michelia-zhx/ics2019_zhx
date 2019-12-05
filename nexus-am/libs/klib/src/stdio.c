#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char temp[1000];
  char* s = temp;
  va_list aptr;
  va_start(aptr, fmt);
  int ret = vsprintf(s, fmt, aptr);
  va_end(aptr);
  for (; *s; s ++) {
      _putc(*s);
  }
  return ret;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int ret = 0;
  int count, num;
  int min_width = 0;
  char *p;
  char numstr[100];
  while (*fmt){
    if (*fmt == '%'){
      fmt += 1;
      char op = *fmt;
      if ('0'<=op && op<='9'){
        while ('0'<=op && op<='9') {
          int dif = op-'0';
          min_width = min_width*10 + dif;
          fmt += 1;
          op = *fmt;
        }
      }
      switch (op) {
        case 'd':{
          num = va_arg(ap, int);
          count = 0;
          if (num < 0){
            *out = '-';
            out += 1;
            num = 0-num;
          }
          else if (num == 0) {
            *out = '0';
            out += 1;
          }
          while (num != 0){
            numstr[count] = num % 10 + '0';
            count += 1;
            num /= 10;
          }
          while (count != 0){
            *out = numstr[count-1];
            count -= 1;
            ret += 1;
            out += 1;
          }
          break;
        }
        
        case 's':{
          p = va_arg(ap, char*);
          while (*p) {
            *out = *p;
            out += 1;
            p += 1;
            ret += 1;
          }
          break;
        }

        case 'p':{
          num = va_arg(ap,int);
        	char result[100];
          int step = 0; 
				  do{
      			result[step]="0123456789abcdef"[num%16];
     				num/=16;
     				step++;
          }while(num);
          result[step] = '\0';
          while(step){
            *out++ = result[step-1];
            step--;
            ret++;
          }
          break;
        }
        
        default:
          break;
      }
      fmt += 1;
    }
    else {
            *out++ = *fmt++;
            ret++;
    }
  }
  *out = '\0';
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
  int iReturn ;
  va_list ap ;
  va_start (ap, fmt) ;
  iReturn = vsprintf (out, fmt, ap) ;
  va_end (ap) ;
  return iReturn ;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
