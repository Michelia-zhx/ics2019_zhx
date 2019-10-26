#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  int ret = 0;
  int count, num;
  char *p;
  char numstr[100];
  while (*fmt){
    if (*fmt == '%'){
      fmt += 1;
      char op = *fmt;
      switch (op) {
      case 'd':
        num = va_arg(ap, int);
        count = 0;
        if (num < 0){
          *out = '-';
          out += 1;
          num = 0-num;
          while (num != 0){
            numstr[count] = num % 10 + '0';
            count += 1;
            num /= 10;
        }
        }
        else if (num == 0) *out = '0';
        else {
          while (num != 0){
            numstr[count] = num % 10 + '0';
            count += 1;
            num /= 10;
          }
        }
        while (count != 0){
          *out = numstr[count-1];
          count -= 1;
          ret += 1;
          out += 1;
        }
        break;
      case 's':
        p = va_arg(ap, char*);
        while (*p) {
          *out = *p;
          out += 1;
          p += 1;
          ret += 1;
        }
        break;
     
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
  va_list pArgs ;
  va_start (pArgs, fmt) ;
  iReturn = vsprintf (out, fmt, pArgs) ;
  va_end (pArgs) ;
  return iReturn ;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
