#include <assert.h>
#include <stdlib.h>
#include <ndl.h>

int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("1\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  printf("2\n");
  assert(bmp->pixels);
  printf("3\n");
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("4\n");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("5\n");
  NDL_Render();
  printf("6\n");
  NDL_CloseDisplay();
  printf("7\n");
 // while (1);
  return 0;
}
