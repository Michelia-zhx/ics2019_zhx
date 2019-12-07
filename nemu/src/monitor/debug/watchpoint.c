#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

int count=0;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  if (count == 0) {
    init_wp_pool();
  }
  WP *p = free_;
  if (p != NULL) {
    free_ = free_->next;
    p -> next = head;
    head = p;
    count++;
    return p;
  }
  else {
    assert(0);
  }
}

int free_wp(int n) {
  WP *p = head;
  if (p == NULL) {
    printf("No watchpoint \n");
    return 0;
  }
  if (head->NO == n) {
    head = head -> next;
    p -> next = free_;
    free_ = p;
    printf("NO.%d Watchpoint deleted\n", n);
    return 1;
  }
  else {
    WP *q;
    while (p -> NO != n) {
      q = p;
      p = p -> next;
    }
    q -> next = p -> next;
    p -> next = free_;
    free_ = p;
    printf("NO.%d Watchpoint deleted\n", n);
    count --;
    return 1;
  }
  return 0;
}

void watchpoint_display() {
  WP *p = head;
  if (p == NULL) {
    printf("No watchpoint \n");
  }
  else {
    while (p !=NULL) {
      printf("%d    %s    %#x\n", p->NO, p->expr, p->old_val);
      p = p -> next;
    }
  }
}

int watchpoint_set(char *s) {
  WP *p = new_wp();
  printf("NO.%d Watchpoint set", p->NO);
  strcpy(head->expr,s);
  bool success = true;
  p->old_val = expr(p->expr,&success);
  if (!success) {
    printf("Fail to Eval\n");
    return 0;
  }
  return 1;
}

WP* check_watchpoint() {
  WP *p = head;
  if (p == NULL) {
   // printf("NO Watchpoint! \n");
    return false;
  }
  else {
    bool success = true;
    while (p != NULL) {
      p -> new_val = expr(p -> expr,&success);
      if (success) {
        p -> old_val = p -> new_val;
        printf("Program stop!\n");
        return p;
      }
    }
  }
  return NULL;
}
