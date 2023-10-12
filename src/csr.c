#include "csr.h"

#include <string.h>

#include "utility.h"

reg_t csrr(CSR* csr, CSR_ADDR addr) { return *(csr->csr + addr); }
void  csrw(CSR* csr, CSR_ADDR addr, reg_t val) { *(csr->csr + addr) = val; }
void  csrs(CSR* csr, CSR_ADDR addr, reg_t val) { *(csr->csr + addr) |= val; }
void  csrc(CSR* csr, CSR_ADDR addr, reg_t val) { *(csr->csr + addr) &= ~val; }

void csr_init(CSR* csr) { memset(csr->csr, 0, sizeof(csr->csr)); }
void csrw_f(CSR* csr, CSR_ADDR addr, uint16_t hi, uint16_t lo, reg_t val) {
  reg_t mask = MASK(hi - lo + 1) << lo;
  val <<= lo;
  reg_t temp = csr->csr[addr];
  temp &= ~mask;
  val &= mask;
  temp |= val;
  csrw(csr, addr, temp);
}
reg_t csrr_f(CSR* csr, CSR_ADDR addr, uint16_t hi, uint16_t lo) {
  return BITS(csrr(csr, addr), hi, lo);
}
