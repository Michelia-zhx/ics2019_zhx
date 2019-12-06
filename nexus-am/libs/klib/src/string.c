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
  char *p = dst;
  while(*src){
    *p = *src;
    src += 1;
    p += 1;
  }
  *p = '\0';
  return dst;
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
}

int strcmp(const char* s1, const char* s2) {
  int a = 0;
  while( (a= (*s1-*s2)) == 0 && *s1 && *s2) {
    s1++;
    s2++;
  }
  return a;
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
  for (int i=0; i < n; i ++) {
    p[i] = c;
  }
  return v;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *csrc = (char*)in, *cdest = (char*)out;
  for (int i = 0; i < n; i ++) {
    cdest[i] = csrc[i];
  }
  return out;
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
