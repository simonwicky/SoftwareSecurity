#include "pngparser.h"
#include <stdio.h>
#include <string.h>

// LibFuzzer stub
//
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  struct image *test_img;

  if(Size == 0 || Size > 255){
      return 0;
  }
      
  char filename[Size + 1];
  filename[Size] = '\0';
  memcpy(filename, Data, Size); 
 
  load_png(filename, &test_img);
  return 0;
}