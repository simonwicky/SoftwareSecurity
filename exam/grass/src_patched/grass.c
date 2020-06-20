#include "inc/grass.h"
#include <stdio.h>

void hijack_flow() {
  printf("Method hijack: Accepted\n");
  fflush(stdout);
}

bool global_flag = false;
void check_corrupt_global() {
  if(global_flag){
    printf((global_flag)? "Global corrupted\n": "Better luck next time\n");
    fflush(stdout);
    global_flag = false;
  }
}