#include "pngparser.h"
#include <stdio.h>
#include <string.h>

// LibFuzzer stub
//
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {

  if(Size == 0){
      return 0;
  }
      

  char filename[Size + 1];
  filename[Size] = '\0';
  memcpy(filename, Data, Size); 
 

  store_png(filename, NULL, NULL, 0);
  remove(filename);
  return 0;
}