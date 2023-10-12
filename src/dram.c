#include "dram.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

DRAM* mk_dram(size_t size) {
  DRAM* ret = malloc(sizeof(DRAM) + size);
  assert(ret);
  ret->size = size;
  return ret;
}

reg_t dram_load(DRAM* dram, reg_t addr, int length) {
  // no validation for whether length is a proper value(1,2,4,8), default is 8
  assert(addr + (length - 1) < dram->size);
  // below code depends on the hypothesis, which is your host isa uses little endian
  switch (length) {
  case 1: return *(byte_t*)(dram->dram + addr);
  case 2: return *(half_t*)(dram->dram + addr);
  case 4: return *(word_t*)(dram->dram + addr);
  default: return *(dword_t*)(dram->dram + addr);
  }
}

void dram_store(DRAM* dram, reg_t addr, int length, reg_t val) {
  // no validation for whether length is a proper value(1,2,4,8), default is 8
  assert(addr + (length - 1) < dram->size);
  // below code depends on the hypothesis, which is your host isa uses little endian
  switch (length) {
  case 1: *(byte_t*)(dram->dram + addr) = val;
  case 2: *(half_t*)(dram->dram + addr) = val;
  case 4: *(word_t*)(dram->dram + addr) = val;
  default: *(dword_t*)(dram->dram + addr) = val;
  }
}
void dd_in(DRAM* dram, reg_t dest_addr, const void* restrict src, size_t size) {
  assert(size <= dram->size - dest_addr);
  memcpy(dram->dram + dest_addr, src, size);
}
