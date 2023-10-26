#include "machine.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bus.h"
#include "hart.h"
#include "plic.h"
#include "uart.h"

#define default_dram_size 1024 * 1024 * 128ull

MACHINE* default_machine() {
  DRAM* dram = mk_dram(default_dram_size);
  bus_init(dram);
  MACHINE* machine = malloc(sizeof(MACHINE));
  assert(machine);
  machine->hart = mk_hart(&bus_instance);

  plic_init(&machine->hart, 1);
  return machine;
}
void dd_bin_dram(MACHINE* machine, const void* restrict src, size_t size) {
  mem_dd(0x80000000, src, size);
}
static void print_machine(MACHINE* machine) {
  printf("hart num: 1\n");
  printf("dram size: %lu Bytes\n", machine->hart->bus->dram->size);
  printf("dram addr(host): %p\n", machine->hart->bus->dram->dram);
  printf("----------\n");
}
void boot(MACHINE* machine) {
  print_machine(machine);
  hart_reset(machine->hart);
  hart_run(machine->hart);
}
