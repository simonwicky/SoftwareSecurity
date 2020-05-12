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

 

  store_png("test_out.png", img, NULL, 0);
  free(px);
  return 0;
}