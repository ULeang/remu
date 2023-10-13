#include "csr.h"

#include <string.h>

#include "utility.h"

reg_t csrr(CSR* csr, CSR_ADDR addr) {
  switch (addr) {
  case sstatus: return csr->csr[mstatus];
  case sie: return csr->csr[mie];
  case sip: return csr->csr[mip];
  default: return csr->csr[addr];
  }
}
void csrw(CSR* csr, CSR_ADDR addr, reg_t val) {
  switch (addr) {
  case sstatus: csr->csr[mstatus] = val; break;
  case sie: csr->csr[mie] = val; break;
  case sip: csr->csr[mip] = val; break;
  default: csr->csr[addr] = val; break;
  }
}
void csrs(CSR* csr, CSR_ADDR addr, reg_t val) {
  switch (addr) {
  case sstatus: csr->csr[mstatus] |= val; break;
  case sie: csr->csr[mie] |= val; break;
  case sip: csr->csr[mip] |= val; break;
  default: csr->csr[addr] |= val; break;
  }
}
void csrc(CSR* csr, CSR_ADDR addr, reg_t val) {
  switch (addr) {
  case sstatus: csr->csr[mstatus] &= ~val; break;
  case sie: csr->csr[mie] &= ~val; break;
  case sip: csr->csr[mip] &= ~val; break;
  default: csr->csr[addr] &= ~val; break;
  }
}

void csr_init(CSR* csr) { memset(csr->csr, 0, sizeof(csr->csr)); }
void csrw_f(CSR* csr, CSR_ADDR addr, uint16_t hi, uint16_t lo, reg_t val) {
  reg_t mask = MASK(hi - lo + 1) << lo;
  val <<= lo;
  reg_t temp = csrr(csr, addr);
  temp &= ~mask;
  val &= mask;
  temp |= val;
  csrw(csr, addr, temp);
}
reg_t csrr_f(CSR* csr, CSR_ADDR addr, uint16_t hi, uint16_t lo) {
  return BITS(csrr(csr, addr), hi, lo);
}
