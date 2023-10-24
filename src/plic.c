#include "plic.h"

#include <assert.h>
#include <stdlib.h>
#include <threads.h>

#include "utility.h"

PLIC plic_instance;
void plic_prepare(HART* harts[], int nharts) {
  assert(nharts <= 8);
  int i = 0;
  for (; i < nharts; ++i) {
    plic_instance.harts[i] = harts[i];
  }
  for (; i < 8; ++i) {
    plic_instance.harts[i] = NULL;
  }
}
uint32_t plic_load(reg_t addr) {
  switch (addr) {
  case 0x000000 ... 0x0000ff:
    return plic_instance.interrupt_priorities_register[BITS(addr, 7, 2)];
  case 0x001000 ... 0x001007:
    return plic_instance.interrupt_pending_bits_register[BITS(addr, 2, 2)];
  case 0x002000 ... 0x00203f:
    return plic_instance.interrupt_enables_register[BITS(addr, 5, 2)];
  case 0x200000 ... 0x207007: {
    int context            = BITS(addr, 15, 12);
    int threshold_or_claim = BITS(addr, 2, 2);
    if (threshold_or_claim == 0) {
      return plic_instance.priority_thresholds_register[context];
    } else {
      uint32_t id = plic_instance.interrupt_claim_completion_register[context];
      plic_instance.interrupt_pending_bits_register[id / 32] &= ~(1u << (id % 32));
      return id;
    }
  }
  default: return 0;
  }
}
void plic_store(reg_t addr, uint32_t value) {
  switch (addr) {
  case 0x000000 ... 0x0000ff:
    plic_instance.interrupt_priorities_register[BITS(addr, 7, 2)] = value;
    break;
  case 0x001000 ... 0x001007:
    plic_instance.interrupt_pending_bits_register[BITS(addr, 2, 2)] = value;
    break;
  case 0x002000 ... 0x00203f:
    plic_instance.interrupt_enables_register[BITS(addr, 5, 2)] = value;
    break;
  case 0x200000 ... 0x207007: {
    int context               = BITS(addr, 15, 12);
    int threshold_or_complete = BITS(addr, 2, 2);
    if (threshold_or_complete == 0) {
      plic_instance.priority_thresholds_register[context] = value;
    } else {
      plic_instance.gateway_pending[value / 32] &= ~(1u << (value % 32));
    }
  } break;
  default: break;
  }
}
static void core_interrupt_signal(uint32_t id) {
  // TODO : not implemented yet
}
void gateway_interrupt_signal(uint32_t id) {
  if (BIT(plic_instance.gateway_pending[id / 32], id % 32) == 0) return;
  plic_instance.gateway_pending[id / 32] |= (1u << (id % 32));
  core_interrupt_signal(id);
}
