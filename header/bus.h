#ifndef __BUS_H__
#define __BUS_H__

#include <stdatomic.h>

#include "dram.h"
#include "plic.h"
#include "uart.h"

enum MEM_MAP {
  PLIC_BASE  = 0xc000000ull,
  PLIC_BOUND = 0xcffffffull,
  UART_BASE  = 0x10000000ull,
  UART_BOUND = 0x10000100ull,
  DRAM_BASE  = 0x80000000ull,
  DRAM_BOUND = 0xffffffffffffffffull,
};

typedef struct BUS {
  // PLIC*       plic;
  // UART*       uart0;
  DRAM*       dram;
  atomic_flag flag;
} BUS;

// singleton instance
extern BUS bus_instance;

extern void  bus_prepare(DRAM* dram);
extern reg_t mem_load(reg_t addr, int length);
extern void  mem_store(reg_t addr, int length, reg_t val);
extern void  mem_dd(reg_t addr, const void* restrict src, size_t size);

#endif
