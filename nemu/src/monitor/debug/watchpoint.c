#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include <stdlib.h>
#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].exp = "\0";
    wp_pool[i].value = 0;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp(char *expression)
{ bool s = 1;
  bool *success = &s; //malloc
  if(head == NULL)
  { WP *f1 = free_;
    WP *f2 = free_;
    while(f1->next != NULL)
      { f2 = f1;
        f1 = f1->next;
      }     // f1:last, f2->next=f1
    head = f1;
    head->exp = expression;
    head->value = expr(expression, success);
    head->next = NULL;
    f2->next = NULL;
    return head;
  }
  else
  {  
     WP *f1 = free_;
     WP *f2 = free_;
     while(f1->next != NULL)
        { f2 = f1;
          f1 = f1->next;
        }     // f1:last, f2->next=f1 
     WP *h = head;
     while(h->next != NULL)
       h = h->next;
     if(h->NO == 0)
       assert(0);
     else
     { h->next = f1;
       h = h->next;
       h->exp = expression;
       h->value = expr(expression, success);
       h->next = NULL;
       f2->next = NULL;
     }
     return h;
  }
}

void free_wp(char *arg)
{ int num = atoi(arg);
  WP *w = head;
  while(NR_WP - w->NO != num)
    w = w->next;
  if(w == NULL)
  printf("No watchpoint numbered %d\n", num);
  if(w == head)
    head = head->next;
  else if(w->next != NULL)
   { WP *wp1 = head;
     while(wp1->next != w)
       wp1 = wp1->next;
     wp1->next = w->next;
   }
  else 
   { WP *wp2 = head;
     while(wp2->next != w)
       wp2 = wp2->next;
     wp2->next = NULL;
   }
  WP *f = free_;
  while(f->next != NULL)
    f = f->next;
  f->next = w;
  w->next = NULL;
  return;
}

void wp_display()
{ if(head == NULL)
    printf("No watchpoint\n");
  else
  { WP *h = head;
    while(h != NULL) 
      { printf("Hardware watchpoint %d: %d\n", NR_WP-h->NO, h->value);
        h = h->next;
      }
  }
  return;
}

bool check_wp()
{ if (head != NULL)
  { WP *h = head;
    bool s = 1;
    bool *success = &s;
    for(; h->next != NULL; h = h->next)
      { uint32_t new_value = expr(h->exp, success);
        if(new_value != h->value)
          { printf("Watchpoint %d: %s\n", h->NO + 1, h->exp);
            printf("Old value = %d\n", h->value);
            printf("New value = %d\n", new_value);
            h->value = new_value;
            return true;
          }
      }
  }  
  return false;
}
