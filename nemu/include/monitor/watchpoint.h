#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  int new_val;
  int old_val;
  char expr[32];
  /* TODO: Add more members if necessary */


} WP;

WP* new_wp();
int free_wp(int n);
void watchpoint_display();
int watchpoint_set(char *s);
WP* check_watchpoint();

#endif
