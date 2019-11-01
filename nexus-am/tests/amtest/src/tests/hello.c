#include <amtest.h>

static inline void putstr(const char *s) {
  for (; *s; s ++) _putc(*s);
}

void hello() {
  for (int i = 0; i < 6; i ++) {
    putstr("Hello, AM World @ " __ISA__ "\n");
  }
}
