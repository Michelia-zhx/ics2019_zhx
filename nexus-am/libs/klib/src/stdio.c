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

char* get_ch(char *dest, int num, int n) {
	char number[20];
	for (int j=0;j<10;j++) {number[j]=j+'0';}
	number[10] = 'a'; number[11] = 'b'; number[12] = 'c'; number[13] = 'd'; number[14] = 'e'; number[15] = 'f';
	int l=0;
	int flag=0;
	if(num==0) {
		dest[0]='0';
		dest[1]='\0';
		return dest;
	}
	if(num<0) {
		flag=1;
		l++;
		num=-num;
	}
	int temp=num;
	while(temp>0) {
		l++;
		temp=temp/n;
	}
	for (int a=0; a<l; a++) {
		int x=num%n;
		assert(x<16);
		dest[l-a-1]=number[x];
		num=num/n;
	}
	if(flag) {dest[0]='-';}
	dest[l]='\0';
	return dest;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap,fmt);
  int sum=0;
  int i=0;
  int len=strlen(fmt);
  out[0]='\0';
  while(i<len) {
    if(fmt[i]=='%') {
      int num=0;
      char ls[50]="";
      switch(fmt[i+1]) {
        case 'd': {
	  		num=va_arg(ap,int);
	  		get_ch(ls, num, 10);
	  		strcat(out,ls);
	  		sum+=strlen(ls);	
        } break;
		case 's': {
	  		strcpy(ls,va_arg(ap, char*));
	  		strcat(out,ls);
	  		sum += strlen(ls);
		} break;
		default: {
			printf("sprintf fault \n");
		}
	  } 
      i+=2;
	}
    else {
      	sum++;
      	char s[2];
      	s[0]=fmt[i];
      	s[1]='\0';
      	strcat(out,s);
      	i++;
  	}
 }
  va_end(ap);
  return sum; 
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
