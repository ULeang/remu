#include "bus.h"

#include <assert.h>
#include <stdlib.h>

BUS* mk_bus(DRAM* dram) {
  BUS* ret = malloc(sizeof(BUS));
  assert(ret);
  ret->dram = dram;
  return ret;
}
reg_t mem_load(BUS* bus, reg_t addr, int length) {
  switch (addr) {
  case DRAM_BASE ... DRAM_BOUND:
    addr -= DRAM_BASE;
    assert(addr < bus->dram->size);
    return dram_load(bus->dram, addr, length);
  default: return 0;
  }
}
void mem_store(BUS* bus, reg_t addr, int length, reg_t val) {
  switch (addr) {
  case DRAM_BASE ... DRAM_BOUND:
    addr -= DRAM_BASE;
    assert(addr < bus->dram->size);
    dram_store(bus->dram, addr, length, val);
    break;
  }
}
void mem_dd(BUS* bus, reg_t addr, const void* restrict src, size_t size) {
  dd_in(bus->dram, addr - DRAM_BASE, src, size);
}
