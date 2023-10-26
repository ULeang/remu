#include "bus.h"

#include <assert.h>
#include <stdlib.h>

#include "uart.h"

BUS   bus_instance = {.flag = ATOMIC_FLAG_INIT};
void  bus_init(DRAM* dram) { bus_instance.dram = dram; }
reg_t mem_load(reg_t addr, int length) {
  reg_t ret;
  switch (addr) {
  case PLIC_BASE ... PLIC_BOUND:
    addr -= PLIC_BASE;
    while (atomic_flag_test_and_set(&bus_instance.flag))
      ;
    ret = plic_load(addr);
    atomic_flag_clear(&bus_instance.flag);
    return ret;
  case UART_BASE ... UART_BOUND:
    addr -= UART_BASE;
    while (atomic_flag_test_and_set(&bus_instance.flag))
      ;
    ret = uart_load(addr);
    atomic_flag_clear(&bus_instance.flag);
    return ret;
  case DRAM_BASE ... DRAM_BOUND:
    addr -= DRAM_BASE;
    while (atomic_flag_test_and_set(&bus_instance.flag))
      ;
    ret = dram_load(bus_instance.dram, addr, length);
    atomic_flag_clear(&bus_instance.flag);
    return ret;
  default: return 0;
  }
}
void mem_store(reg_t addr, int length, reg_t val) {
  switch (addr) {
  case PLIC_BASE ... PLIC_BOUND:
    addr -= PLIC_BASE;
    while (atomic_flag_test_and_set(&bus_instance.flag))
      ;
    plic_store(addr, val);
    atomic_flag_clear(&bus_instance.flag);
    break;
  case UART_BASE ... UART_BOUND:
    addr -= UART_BASE;
    while (atomic_flag_test_and_set(&bus_instance.flag))
      ;
    uart_store(addr, val);
    atomic_flag_clear(&bus_instance.flag);
    break;
  case DRAM_BASE ... DRAM_BOUND:
    addr -= DRAM_BASE;
    while (atomic_flag_test_and_set(&bus_instance.flag))
      ;
    dram_store(bus_instance.dram, addr, length, val);
    atomic_flag_clear(&bus_instance.flag);
    break;
  default: break;
  }
}
void mem_dd(reg_t addr, const void* restrict src, size_t size) {
  dd_in(bus_instance.dram, addr - DRAM_BASE, src, size);
}
