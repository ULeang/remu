#ifndef __DRAM_H__
#define __DRAM_H__

#include <stddef.h>

#include "type.h"

typedef struct DRAM {
  size_t size;
  byte_t dram[0];
} DRAM;

extern DRAM* mk_dram(size_t size);
extern reg_t dram_load(DRAM* dram, reg_t addr, int length);
extern void  dram_store(DRAM* dram, reg_t addr, int length, reg_t val);
extern void  dd_in(DRAM* dram, reg_t dest_addr, const void* restrict src,
                   size_t size);

#endif
