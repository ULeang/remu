#include <stdio.h>

#include "machine.h"
#include "monitor.h"

int main(int argc, char** argv) {
  if (argc == 1) {
    fprintf(stderr, "no file provided\n");
    return 1;
  }
  FILE* fp = fopen(argv[1], "rb");
  if (!fp) {
    fprintf(stderr, "cannot open '%s'\n", argv[1]);
    return 1;
  }
  byte_t buffer[512] = {0};
  fread(buffer, 512, 1, fp);
  fclose(fp);

  MACHINE* machine = default_machine();
  dd_bin_dram(machine, buffer, 512);
  // boot(machine);
  hart_reset(machine->hart);

  initialize_readline();
  char* input = NULL;
  while (1) {
    input = rl();
    if (!input) break;
    monitor_execute(machine, input);
  }
  return 0;
}
