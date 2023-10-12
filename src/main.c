#include <stdio.h>

#include "machine.h"

int main() {
  FILE* fp = fopen("hello.bin", "rb");
  if (!fp) {
    fprintf(stderr, "cannot open 'hello.bin'\n");
    return 1;
  }
  byte_t buffer[512] = {0};
  fread(buffer, 512, 1, fp);
  fclose(fp);

  MACHINE* machine = default_machine();
  dd_bin_dram(machine, buffer, 512);
  boot(machine);
  return 0;
}
