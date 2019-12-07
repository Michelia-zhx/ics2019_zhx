#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#include <string.h>

#define NR_WP 32
#define LEN_WP_NAME 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL; // head用于组织使用中的监视点结构, free_用于组织空闲的监视点结构

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].exp[0] = '\0';
    wp_pool[i].value = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void new_wp(char *args){
  WP *p;
  if (free_ == NULL){
    panic("No more watchpoints.");
  }
  else {
    p = free_;
    free_ = p->next;
    p->next = NULL;
    if (head == NULL){
      head = p;
    }
    else {
      WP *q = head;
      while (q->next!=NULL) q = q->next;
      q->next = p;
    }
    strncpy(p->exp, args, LEN_WP_NAME);
    bool success = true;
    p->value = expr(args, &success);
    Log("Watch point %d is built, exp is %s, value is %d.", p->NO, p->exp, p->value);
  }
  return;
}

void free_wp(WP *wp){
  Log("Free watch point %d.", wp->NO);
  wp->next = NULL;
  if (free_ == NULL){
    free_ = wp;
  }
  else {
    WP *p = free_;
    while (p->next!=NULL) p = p->next;
    p->next = wp;
  }
  return;
}

void delete_wp(int num){
  WP *p = head;
  bool is_found = false;
  if (p->NO == num){
    is_found = true;
    head = head->next;
    free_wp(p);
  }
  else {
    while (p->next!=NULL){
      if (p->next->NO == num){
        is_found = true;
        WP *temp = p->next;
        p->next = temp->next;
        free_wp(temp);
        break;
      }
      p = p->next;
    }
  }
  if (is_found == false){
    Log("Can't find watch point %d.\n", num);
  }
  return;
}

void info_wp_display(){
  WP *p = head;
  printf("NO\texpression\tcurrent value\t\n");
  if (head == NULL){
    printf("No watch point is in use.\n");
  }
  else {
    printf("%d\t%s\t%d\t\n", p->NO, p->exp, p->value);
    while (p->next != NULL){
      p = p->next;
      printf("%d\t%s\t%d\t\n", p->NO, p->exp, p->value);
    }
  }
  return;
}

bool check(){
  WP *p = head;
  if (p != NULL){
    bool flag = false;
    bool success = true;
    while (p->next != NULL){
      int cur_value = expr(p->exp, &success);
      if (cur_value != p->value){
        flag = true;
        Log("Value of watch point %d has changed\nOld value: %d\nNew value: %d\n", p->NO, p->value, cur_value);
      }
      p = p->next;
    }
    if (flag == true) return true;
    else return false;
  }
  return false; 
}