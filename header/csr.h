#ifndef __CSR_H__
#define __CSR_H__

#include "csr_list.h"
#include "type.h"

typedef struct CSR {
  reg_t csr[4096];
} CSR;

/*
 * for simplicity and performance, remu provides NO GUARANTEE of WARL fields of CSRs,
 * instead, remu simply implements it as if WLRL fields
 */

extern reg_t csrr(CSR* csr, CSR_ADDR addr);
extern void  csrw(CSR* csr, CSR_ADDR addr, reg_t val);
extern void  csrs(CSR* csr, CSR_ADDR addr, reg_t val);
extern void  csrc(CSR* csr, CSR_ADDR addr, reg_t val);

extern void  csrw_f(CSR* csr, CSR_ADDR addr, uint16_t hi, uint16_t lo, reg_t val);
extern reg_t csrr_f(CSR* csr, CSR_ADDR addr, uint16_t hi, uint16_t lo);

extern void csr_init(CSR* csr);

#endif
