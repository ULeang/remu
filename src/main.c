#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "machine.h"
#include "monitor.h"

#define fatal(exit_code, fmt, args...)                     \
  ({                                                       \
    fprintf(stderr, "\033[1;31merror\033[m:" fmt, ##args); \
    exit(exit_code);                                       \
  })
#define usage(exit_code, fmt, args...)                     \
  ({                                                       \
    fprintf(stdout, "\033[1;34musage\033[m:" fmt, ##args); \
    exit(exit_code);                                       \
  })

int main(int argc, char** argv) {
  // -f bin_file_name
  char* bin_file_name = NULL;
  // -m monitor
  enum { MONITOR_MODE, RUN_MODE } launch_mode = RUN_MODE;

  for (int i = 1; i < argc; ++i) {
    char* argi = argv[i];
    if (strcmp("-f", argi) == 0) {
      ++i;
      if (i == argc) fatal(1, "no arg provided for '-f' option\n");
      bin_file_name = argv[i];
    } else if (strcmp("-m", argi) == 0) {
      ++i;
      if (i == argc) {
        fatal(1, "no arg provided for '-m' option\n");
      } else if (strcmp("monitor", argv[i]) == 0) {
        launch_mode = MONITOR_MODE;
      } else if (strcmp("no-monitor", argv[i]) == 0) {
        launch_mode = RUN_MODE;
      } else {
        fatal(1, "unrecognized arg '%s' for '-m' option\n", argv[i]);
      }
    } else if (strcmp("-h", argi) == 0) {
      usage(0, "\n\t-f\tfile_name\n\t-m\t[monitor|no-monitor]\n\t-h\n");
    } else {
      fatal(1, "unrecognized option '%s'\n", argi);
    }
  }

  if (bin_file_name == NULL) fatal(2, "no file provided\n");

  FILE* fp = fopen(bin_file_name, "rb");
  if (!fp) {
    fatal(2, "cannot open file '%s'\n", bin_file_name);
  }
  byte_t buffer[512] = {0};
  fread(buffer, 512, 1, fp);
  fclose(fp);

  if (launch_mode == MONITOR_MODE) {
    MACHINE* machine = monitor_machine();
    dd_bin_dram(machine, buffer, 512);
    hart_reset(machine->hart);
    initialize_readline();
    char* input = NULL;
    while (1) {
      input = rl();
      if (!input) break;
      monitor_execute(machine, input);
    }
    return 0;
  } else if (launch_mode == RUN_MODE) {
    MACHINE* machine = default_machine();
    dd_bin_dram(machine, buffer, 512);
    hart_reset(machine->hart);
    boot(machine);
    return 0;
  }
}
