#include "bus.h"
#include "dram.h"
#include "hart.h"
#include "type.h"

typedef struct MACHINE {
  HART* hart;
} MACHINE;

extern MACHINE* default_machine();
extern void     dd_bin_dram(MACHINE* machine, const void* restrict src, size_t size);
extern void     boot(MACHINE* machine);
