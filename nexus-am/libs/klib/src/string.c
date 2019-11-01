#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  const char *p = s;
  size_t len = 0;
  for(; *p != '\0'; p++)
    len++;
  return len;
}

char *strcpy(char* dst,const char* src) {
  return strncpy(dst, src, strlen(src));
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t len = strlen(src);
  if(len < n)
    return strcpy(dst, src);
  else{
    for(int i = 0; i < n; i++)
      *(dst + i) = *(src + i);
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  char *head = dst;
  while(*dst != '\0')
    dst++;
  strcpy(dst, src);
  return head;
}

int strcmp(const char* s1, const char* s2) {
  while(*s1 == *s2){
    s1++;
    s2++;
    if(*s1 == '\0' && *s2 == '\0')
      return 0;
  }
  if(*s1 < *s2)
    return -1;
  else
    return 1;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  size_t len = strlen(s2);
  if(len <= n)
    return strcmp(s1, s2);
  else{
    for(int i = 0; i < n;){
      while(*s1 == *s2){
        s1++;
        s2++;
        i++;
        if(*s1 == '\0' || *s2 == '\0')
          return 0; 
      }
      if(*s1 < *s2)
        return -1;
      else 
        return 1;
    }
  }
  return 0;
}

void* memset(void* v,int c,size_t n) {
  char * v1;
  v1 = (char*)v;
  char c1 = '0' + c - 0;
  for(int i = 0; i < n; i++){
    *v1 = c1;
    v1++;
  }
  v1 = (void*)v1;
  return v1;
}

void* memcpy(void* out, const void* in, size_t n) {
  char *out1;
  const char *in1;
  out1 = (char*)out;
  in1 = (char*)in;
  for(int i = 0; i < n; i++){
    *out1 = *in1;
    out1 ++;
    in1 ++;
  }
  out1 = (void*)out1;
  return out1;
}

int memcmp(const void* s1, const void* s2, size_t n){
  /*
  const char *s1_, *s2_;
  s1_ = (char*)s1;
  s2_ = (char*)s2;
  for(int i = 0; i < n;){
    while(*s1_ == *s2_){
      s1_++;
      s2_++;
      i++;
    }
    if(*s1_ < *s2_)
      return -1;
    else 
      return 1;
  }*/
  return 0; 
}

#endif
