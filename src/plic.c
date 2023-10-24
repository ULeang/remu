#include "plic.h"

#include "utility.h"

uint32_t plic_load(PLIC* plic, reg_t addr) {
  switch (addr) {
  case 0x000000 ... 0x0000ff:
    return plic->interrupt_priorities_register[BITS(addr, 7, 2)];
  case 0x001000 ... 0x001007:
    return plic->interrupt_pending_bits_register[BITS(addr, 2, 2)];
  case 0x002000 ... 0x00203f:
    return plic->interrupt_enables_register[BITS(addr, 5, 2)];
  case 0x200000 ... 0x207007: {
    int context            = BITS(addr, 15, 12);
    int threshold_or_claim = BITS(addr, 2, 2);
    if (threshold_or_claim == 0) {
      return plic->priority_thresholds_register[context];
    } else {
      uint32_t id = plic->interrupt_claim_completion_register[context];
      plic->interrupt_pending_bits_register[id / 32] &= ~(1ull << (id % 32));
      return id;
    }
  }
  default: return 0;
  }
}
void plic_store(PLIC* plic, reg_t addr, uint32_t value) {
  switch (addr) {
  case 0x000000 ... 0x0000ff:
    plic->interrupt_priorities_register[BITS(addr, 7, 2)] = value;
    break;
  case 0x001000 ... 0x001007:
    plic->interrupt_pending_bits_register[BITS(addr, 2, 2)] = value;
    break;
  case 0x002000 ... 0x00203f:
    plic->interrupt_enables_register[BITS(addr, 5, 2)] = value;
    break;
  case 0x200000 ... 0x207007: {
    int context               = BITS(addr, 15, 12);
    int threshold_or_complete = BITS(addr, 2, 2);
    if (threshold_or_complete == 0) {
      plic->priority_thresholds_register[context] = value;
    } else {
      plic->gateway_pending[value / 32] &= ~(1ull << (value % 32));
    }
  } break;
  default: break;
  }
}
