#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

#define LEN_WP_NAME 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char exp[LEN_WP_NAME];
  uint32_t value;

} WP;

void new_wp();
void free_wp(WP *wp);
void delete_wp(int num);
void info_wp_display();
bool check();

#endif
