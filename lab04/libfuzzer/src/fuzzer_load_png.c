#include "pngparser.h"
#include <stdio.h>

// LibFuzzer stub
//
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  struct image *test_img;

  FILE *input = fopen("testfile.png","w");
  fwrite(Data, Size, 1, input);
  fclose(input);
  
  // What would happen if we run multiple fuzzing processes at the same time?
  // Take a look at the name of the file.
  load_png("testfile.png", &test_img);

  // Always return 0
  return 0;
}
