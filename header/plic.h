#ifndef __PLIC_H__
#define __PLIC_H__

#include "hart.h"

typedef struct PLIC {
  HART*    harts[8];
  uint32_t gateway_pending[2];
  uint32_t interrupt_priorities_register[64];
  uint32_t interrupt_pending_bits_register[2];
  uint32_t interrupt_enables_register[2 * 8];
  uint32_t priority_thresholds_register[8];
  uint32_t interrupt_claim_completion_register[8];
} PLIC;

extern uint32_t plic_load(PLIC* plic, reg_t addr);
extern void     plic_store(PLIC* plic, reg_t addr, uint32_t value);

#endif
