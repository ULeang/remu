#ifndef __BUS_H__
#define __BUS_H__

#include "dram.h"
#include "plic.h"
#include "uart.h"

enum MEM_MAP {
  DRAM_BASE  = 0x80000000ull,
  DRAM_BOUND = 0xffffffffffffffffull,
};

typedef struct BUS {
  DRAM* dram;

} BUS;

extern BUS*  mk_bus(DRAM* dram);
extern reg_t mem_load(BUS* bus, reg_t addr, int length);
extern void  mem_store(BUS* bus, reg_t addr, int length, reg_t val);
extern void  mem_dd(BUS* bus, reg_t addr, const void* restrict src, size_t size);

#endif
