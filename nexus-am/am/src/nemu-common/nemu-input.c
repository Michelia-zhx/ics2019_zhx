#include <am.h>
#include <amdev.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000
unsigned long long press;
size_t __am_input_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_INPUT_KBD: {
      _DEV_INPUT_KBD_t *kbd = (_DEV_INPUT_KBD_t *)buf;
      press = inl(KBD_ADDR);
      kbd->keycode = press;
      if (press != _KEY_NONE){
        kbd->keydown = !(kbd->keydown);
      }
      return sizeof(_DEV_INPUT_KBD_t);
    }
  }
  return 0;
}
