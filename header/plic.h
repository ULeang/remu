#ifndef __PLIC_H__
#define __PLIC_H__

#include <stdatomic.h>

#include "type.h"

typedef struct HART HART;

typedef struct PLIC {
  HART*    harts[8];
  uint32_t gateway_pending[2];
  uint32_t interrupt_priorities_register[64];
  uint32_t interrupt_pending_bits_register[2];
  uint32_t interrupt_enables_register[2 * 8];
  uint32_t priority_thresholds_register[8];
  uint32_t interrupt_claim_completion_register[8];
} PLIC;

// singleton instance
extern PLIC plic_instance;

extern void     plic_prepare(HART* harts[], int nharts);
extern uint32_t plic_load(reg_t addr);
extern void     plic_store(reg_t addr, uint32_t value);
extern void     gateway_interrupt_signal(uint32_t id);

#endif
