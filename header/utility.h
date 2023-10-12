#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "type.h"

static inline reg_t MASK(int n) { return n >= 64 ? ~0ull : (1ull << n) - 1; }
static inline reg_t BITS(reg_t imm, int hi, int lo) {
  return (imm & MASK(hi + 1)) >> lo;
}
static inline reg_t BIT(reg_t imm, int bit) { return (imm >> bit) & 1ull; }
static inline reg_t SEXT(reg_t imm, int n) {
  if ((imm >> (n - 1)) & 1) {
    return ((~0ull) << n | imm);
  } else {
    return imm & MASK(n);
  }
}

#endif
