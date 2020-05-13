#include "pngparser.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// LibFuzzer stub
//
int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
  struct image *test_img;

  pid_t pid = getpid();
  char filename[18];
  sprintf(filename,"testfile%d.png", pid);
  FILE *input = fopen(filename,"w");
  fwrite(Data, Size, 1, input);
  fclose(input);
  
  // What would happen if we run multiple fuzzing processes at the same time?
  // Take a look at the name of the file.
  load_png(filename, &test_img);

  // Always return 0
  return 0;
}
