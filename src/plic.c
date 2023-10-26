#include "plic.h"

#include <assert.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>
#include <stdio.h>

#include "csr.h"
#include "hart.h"
#include "utility.h"

PLIC plic_instance = {.flag = ATOMIC_FLAG_INIT};
void plic_init(HART* harts[], int nharts) {
  assert(nharts <= 8);
  int i = 0;
  for (; i < nharts; ++i) {
    plic_instance.harts[i] = harts[i];
  }
  for (; i < 8; ++i) {
    plic_instance.harts[i] = NULL;
  }
}
static void complete(uint32_t id) {
  while (atomic_flag_test_and_set(&plic_instance.flag))
    ;
  *(uint64_t*)plic_instance.gateway_pending &= ~(1ull << id);
  atomic_flag_clear(&plic_instance.flag);
}
// must be invoked under a lock
static void notify() {
  uint32_t highest_priority = 0, id = 0;
  uint64_t pending = *(uint64_t*)plic_instance.interrupt_pending_bits_register;
  for (int i = 0; i < 64; ++i) {
    _Bool ispending = pending & 0x1ull;
    pending >>= 1;
    uint32_t priority = plic_instance.interrupt_priorities_register[i];
    if (ispending && priority > highest_priority) {
      highest_priority = priority;
      id               = i;
    }
  }
  if (id == 0) return;
  for (int i = 0; i < 8 && plic_instance.harts[i] != NULL; ++i) {
    _Bool isenabled =
        BIT(*(uint64_t*)plic_instance.interrupt_enables_register + (i * 2), id);
    if (isenabled &&
        highest_priority > plic_instance.priority_thresholds_register[i]) {
      HART_PRL_MODE prl = plic_instance.harts[i]->privilege;
      if (prl == M) {
        csrw_f(&plic_instance.harts[i]->csr, mip, 11, 11, 1);
      } else {
        csrw_f(&plic_instance.harts[i]->csr, mip, 9, 9, 1);
      }
    } else {
      csrw_f(&plic_instance.harts[i]->csr, mip, 9, 9, 0);
      csrw_f(&plic_instance.harts[i]->csr, mip, 11, 11, 0);
    }
  }
}
static uint32_t claim(int context) {
  while (atomic_flag_test_and_set(&plic_instance.flag))
    ;
  uint32_t highest_priority = 0, id = 0;
  uint64_t pending   = *(uint64_t*)plic_instance.interrupt_pending_bits_register;
  uint32_t threshold = plic_instance.priority_thresholds_register[context];
  for (int i = 0; i < 64; ++i) {
    uint32_t ispending = pending & 0x1ull;
    uint32_t priority  = plic_instance.interrupt_priorities_register[i];
    pending >>= 1;
    if (ispending && priority > threshold && priority > highest_priority) {
      highest_priority = priority;
      id               = i;
    }
  }
  *(uint64_t*)plic_instance.interrupt_pending_bits_register &= ~(1ull << id);
  notify();
  atomic_flag_clear(&plic_instance.flag);
  return id;
}
uint32_t plic_load(reg_t addr) {
  switch (addr) {
  case 0x000000 ... 0x0000ff:
    return plic_instance.interrupt_priorities_register[BITS(addr, 7, 2)];
  case 0x001000 ... 0x001007:
    return plic_instance.interrupt_pending_bits_register[BIT(addr, 2)];
  case 0x002000 ... 0x00203f:
    return plic_instance.interrupt_enables_register[BITS(addr, 5, 2)];
  case 0x200000 ... 0x207007: {
    int context            = BITS(addr, 15, 12);
    int threshold_or_claim = BIT(addr, 2);
    if (threshold_or_claim == 0) {
      return plic_instance.priority_thresholds_register[context];
    } else {
      return claim(context);
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
  /* case 0x001000 ... 0x001007:
    plic_instance.interrupt_pending_bits_register[BIT(addr, 2)] = value;
    break; */
  case 0x002000 ... 0x00203f:
    plic_instance.interrupt_enables_register[BITS(addr, 5, 2)] = value;
    break;
  case 0x200000 ... 0x207007: {
    int context               = BITS(addr, 15, 12);
    int threshold_or_complete = BIT(addr, 2);
    if (threshold_or_complete == 0) {
      plic_instance.priority_thresholds_register[context] = value;
    } else {
      complete(value);
    }
  } break;
  default: break;
  }
}
void gateway_interrupt_signal(uint32_t id) {
  while (atomic_flag_test_and_set(&plic_instance.flag))
    ;
  if (BIT(*(uint64_t*)plic_instance.gateway_pending, id) == 1) {
    // notify();//TODO : for debug
    atomic_flag_clear(&plic_instance.flag);
    return;
  }
  *(uint64_t*)plic_instance.gateway_pending |= (1ull << id);
  *(uint64_t*)plic_instance.interrupt_pending_bits_register |= (1ull << id);
  notify();
  atomic_flag_clear(&plic_instance.flag);
}
