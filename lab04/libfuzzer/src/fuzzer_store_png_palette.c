#include "pngparser.h"
#include <stdio.h>
#include <string.h>

// LibFuzzer stub
//
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  if(Size < 10000) {
    return 0;
  }
  
  struct image *img;
  void* px = malloc(sizeof(uint8_t) * Size);
  memcpy(px, Data, Size);

  img->size_x = 100;
  img->size_y = 100;
      
  img->px = (struct pixel *)px;

 struct pixel palette[] = {
    {
      .red = 255,
      .green = 255,
      .blue = 255,
      .alpha = 255
    },
    {
      .red = 0,
      .green = 0,
      .blue = 0,
      .alpha = 255
    },
    {
      .red = 255,
      .green = 0,
      .blue = 0,
      .alpha = 255
    },
    {
      .red = 0,
      .green = 255,
      .blue = 0,
      .alpha = 255
    },
    {
      .red = 0,
      .green = 0,
      .blue = 255,
      .alpha = 255
    },
 };

  store_png("test_out.png", img, palette, 5);
  free(px);
  return 0;
}