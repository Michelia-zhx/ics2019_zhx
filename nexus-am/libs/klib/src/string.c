#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t length = 0;
  while(*s){
    length += 1;
    s += 1;
  }
  return length;
}

char *strcpy(char* dst,const char* src) {
  /*
  char *p = dst;
  while(*src){
    *p = *src;
    src += 1;
    p += 1;
  }
  *p = '\0';
  return dst;
  */
  char* temp = dst;
	while(*src) *dst++ = *src++; 
	*dst = '\0';
	return temp;
  
}

char* strncpy(char* dst, const char* src, size_t n) {
  char *p = dst;
  for (int i=1; i<=n; ++i){
    if (*src){
      *p = *src;
      src += 1;
      p += 1;
    }
    else{
      *p = '\0';
      p += 1;
    }
  }
  if (*src) *p = '\0';
  return dst;
}

char* strcat(char* dst, const char* src) {
  
  char *p = dst;
  while (*p) p += 1;
  while (*src){
    *p = *src;
    p += 1;
    src += 1;
  }
  *p = '\0';
  return dst;
  /*
 char* temp = dst;
	while(*dst) dst++;
	while(*src) *dst++ = *src++;
	*dst = '\0';
	return temp;
 */ 
}

int strcmp(const char* s1, const char* s2) {
  
  int result = 0;
  while (*s1 && *s2 && (*s1 == *s2)){
    s1 += 1;
    s2 += 1;
  }
  if (!(*s1) && !(*s2)) result = 0;
  else if (!(*s1) && *s2) result = -1;
  else if (*s1 && !(*s2)) result = 1;
  else if (*s1 < *s2) result = -1;
  else if (*s1 > *s2) result = 1;
  else {
    printf("error in atrcmp");
    assert(0);
  }
  return result;
  /*
 while(*s1 && *s2 && (*s1 == *s2)){
    s1++;
	s2++;
  }
  if(*s1 > *s2) return 1;
  else if(*s1 < *s2) return -1;
  else return 0;
  */
}

int strncmp(const char* s1, const char* s2, size_t n) {
  /*
  int result = 0;
  char *p1 = s1;
  char *p2 = s2;
  n -=1;
  while (*p1 && *p2 && n && (*p1==*p2)){
      p1 += 1;
      p2 += 1;
      n -= 1;
  }
  if (!(*p1) && !(*p2)) result = 0;
  else if (!(*p1) && *p2) result = -1;
  else if (*p1 && !(*p2)) result = 1;
  else if (*p1 > *p2) result = 1;
  else if (*p1 < *p2) result = -1;
  else result = 0;
  return result;
  */
 return 0;
}

void* memset(void* v,int c,size_t n) {
  
  char *p = (char *)v;
  for (int i=0; i < n; ++i) {
    *p = c;
    p += 1;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  void *origin = out;
  char *p = (char *)out;
  char *q = (char *)in;
  while (n > 0){
    *p = *q;
    p += 1;
    q += 1;
    n--;
  }
  return origin;
}

int memcmp(const void* s1, const void* s2, size_t n){
  
  int result = 0;
  char *p1 = (char *)s1;
  char *p2 = (char *)s2;
  n -=1;
  while (*p1 && *p2 && n && (*p1==*p2)){
      p1 += 1;
      p2 += 1;
      n -= 1;
  }
  if (!(*p1) && !(*p2)) result = 0;
  else if (!(*p1) && *p2) result = -1;
  else if (*p1 && !(*p2)) result = 1;
  else if (*p1 > *p2) result = 1;
  else if (*p1 < *p2) result = -1;
  else result = 0;
  return result;
}

#endif
