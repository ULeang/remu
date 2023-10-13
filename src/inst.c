#include "inst.h"

#include "csr.h"
#include "hart.h"
#include "utility.h"

#define RD    (hart->gpr[hart->decoder.rd])
#define RS1   (hart->gpr[hart->decoder.rs1])
#define RS2   (hart->gpr[hart->decoder.rs2])
#define RS1S  ((sreg_t)(hart->gpr[hart->decoder.rs1]))
#define RS2S  ((sreg_t)(hart->gpr[hart->decoder.rs2]))
#define IMM   (hart->decoder.imm)
#define CPC   (hart->decoder.cpc)
#define NPC   (hart->decoder.npc)
#define SHAMT (hart->decoder.shamt)
#define CSR   (hart->decoder.csr_addr)

#define PRL (hart->privilege)

#define LOAD(addr, length) \
  (SEXT(mem_load(hart->bus, (addr), (length)), (length) << 3))
#define LOADU(addr, length)        (mem_load(hart->bus, (addr), (length)))
#define STORE(addr, length, value) (mem_store(hart->bus, (addr), (length), (value)))

#define CSRR(addr)        (csrr(&hart->csr, (addr)))
#define CSRW(addr, value) (csrw(&hart->csr, (addr), (value)))
#define CSRS(addr, value) (csrs(&hart->csr, (addr), (value)))
#define CSRC(addr, value) (csrc(&hart->csr, (addr), (value)))

#define CSRW_F(addr_hi_lo_val...) (csrw_f(&hart->csr, ##addr_hi_lo_val))
#define CSRR_F(addr_hi_lo...)     (csrr_f(&hart->csr, ##addr_hi_lo))
#define mstatus_MPIE              mstatus, 7, 7
#define mstatus_MIE               mstatus, 3, 3
#define mstatus_MPP               mstatus, 12, 11
#define mstatus_SPIE              mstatus, 5, 5
#define mstatus_SIE               mstatus, 1, 1
#define mstatus_SPP               mstatus, 8, 8
#define mstatus_TSR               mstatus, 22, 22
#define sstatus_SIE               sstatus, 1, 1
#define sstatus_SPIE              sstatus, 5, 5
#define mtvec_MODE                mtvec, 1, 0
#define stvec_MODE                stvec, 1, 0
#define mstatus_MPRV              mstatus, 17, 17

#define raise_exception_if(cond, exception_code) \
  ({                                             \
    if (cond) {                                  \
      raise_exception(hart, (exception_code));   \
      return;                                    \
    }                                            \
  })

#define read_only_flag  (BITS(CSR, 11, 10) == 0b11)
#define privilege_flag  (BITS(CSR, 9, 8))
#define inst_read_flag  (hart->decoder.rd != 0)
#define inst_write_flag (hart->decoder.rs1 != 0)

static void raise_exception(HART* hart, EXCEPTION_CODE exception_code) {
  reg_t trap_code = exception_code;
  if (PRL <= S && CSRR_F(medeleg, exception_code, exception_code)) {
    CSRW(scause, trap_code);
    CSRW(sepc, CPC);
    CSRW(stval, 0);
    CSRW_F(mstatus_SPP, PRL);
    CSRW_F(mstatus_SPIE, CSRR_F(mstatus_SIE));
    CSRW_F(mstatus_SIE, 0);
    PRL = S;
    NPC = CSRR(stvec) & ~0b11ull;
  } else {
    CSRW(mcause, trap_code);
    CSRW(mepc, CPC);
    CSRW(mtval, 0);
    CSRW_F(mstatus_MPP, PRL);
    CSRW_F(mstatus_MPIE, CSRR_F(mstatus_MIE));
    CSRW_F(mstatus_MIE, 0);
    PRL = M;
    NPC = CSRR(mtvec) & ~0b11ull;
  }
}

void _lui(HART* hart) { RD = IMM; }
void _auipc(HART* hart) { RD = CPC + IMM; }
void _jal(HART* hart) {
  reg_t target = CPC + IMM;
  raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
  RD  = NPC;
  NPC = target;
}
void _jalr(HART* hart) {
  reg_t target = (RS1 + IMM) & ~1ull;
  raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
  RD  = NPC;
  NPC = target;
}
void _beq(HART* hart) {
  reg_t target = CPC + IMM;
  if (RS1 == RS2) {
    raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
    NPC = target;
  }
}
void _bne(HART* hart) {
  reg_t target = CPC + IMM;
  if (RS1 != RS2) {
    raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
    NPC = target;
  }
}
void _blt(HART* hart) {
  reg_t target = CPC + IMM;
  if (RS1S < RS2S) {
    raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
    NPC = target;
  }
}
void _bge(HART* hart) {
  reg_t target = CPC + IMM;
  if (RS1S >= RS2S) {
    raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
    NPC = target;
  }
}
void _bltu(HART* hart) {
  reg_t target = CPC + IMM;
  if (RS1 < RS2) {
    raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
    NPC = target;
  }
}
void _bgeu(HART* hart) {
  reg_t target = CPC + IMM;
  if (RS1 >= RS2) {
    raise_exception_if(BITS(target, 1, 0), Instruction_address_misaligned);
    NPC = target;
  }
}
void _lb(HART* hart) { RD = LOAD(RS1 + IMM, 1); }
void _lh(HART* hart) { RD = LOAD(RS1 + IMM, 2); }
void _lw(HART* hart) { RD = LOAD(RS1 + IMM, 4); }
void _lbu(HART* hart) { RD = LOADU(RS1 + IMM, 1); }
void _lhu(HART* hart) { RD = LOADU(RS1 + IMM, 2); }
void _sb(HART* hart) { STORE(RS1 + IMM, 1, RS2); }
void _sh(HART* hart) { STORE(RS1 + IMM, 2, RS2); }
void _sw(HART* hart) { STORE(RS1 + IMM, 4, RS2); }
void _addi(HART* hart) { RD = RS1 + IMM; }
void _slti(HART* hart) { RD = RS1S < IMM ? 1 : 0; }
void _sltiu(HART* hart) { RD = RS1 < IMM ? 1 : 0; }
void _xori(HART* hart) { RD = RS1 ^ IMM; }
void _ori(HART* hart) { RD = RS1 | IMM; }
void _andi(HART* hart) { RD = RS1 & IMM; }
void _slli(HART* hart) { RD = RS1 << SHAMT; }
void _srli(HART* hart) { RD = RS1 >> SHAMT; }
void _srai(HART* hart) { RD = RS1S >> SHAMT; }
void _add(HART* hart) { RD = RS1 + RS2; }
void _sub(HART* hart) { RD = RS1 - RS2; }
void _sll(HART* hart) { RD = RS1 << BITS(RS2, 5, 0); }
void _slt(HART* hart) { RD = RS1S < RS2S ? 1 : 0; }
void _sltu(HART* hart) { RD = RS1 < RS2 ? 1 : 0; }
void _xor(HART* hart) { RD = RS1 ^ RS2; }
void _srl(HART* hart) { RD = RS1 >> BITS(RS2, 5, 0); }
void _sra(HART* hart) { RD = RS1S >> BITS(RS2, 5, 0); }
void _or(HART* hart) { RD = RS1 | RS2; }
void _and(HART* hart) { RD = RS1 & RS2; }
void _fence(HART* hart) {}
void _ecall(HART* hart) {
  raise_exception(hart, Environment_call_from_U_mode + PRL);
}
void _ebreak(HART* hart) { raise_exception(hart, Breakpoint); }

void _lwu(HART* hart) { RD = LOADU(RS1 + IMM, 4); }
void _ld(HART* hart) { RD = LOADU(RS1 + IMM, 8); }
void _sd(HART* hart) { STORE(RS1 + IMM, 8, RS2); }
void _addiw(HART* hart) { RD = SEXT(RS1 + IMM, 32); }
void _slliw(HART* hart) { RD = SEXT(RS1 << SHAMT, 32); }
void _srliw(HART* hart) { RD = SEXT(BITS(RS1, 31, 0) >> SHAMT, 32); }
void _sraiw(HART* hart) { RD = ((sreg_t)SEXT(RS1, 32)) >> SHAMT; }
void _addw(HART* hart) { RD = SEXT(RS1 + RS2, 32); }
void _subw(HART* hart) { RD = SEXT(RS1 - RS2, 32); }
void _sllw(HART* hart) { RD = SEXT(RS1 << BITS(RS2, 4, 0), 32); }
void _srlw(HART* hart) { RD = SEXT(BITS(RS1, 31, 0) >> BITS(RS2, 4, 0), 32); }
void _sraw(HART* hart) { RD = ((sreg_t)SEXT(RS1, 32)) >> BITS(RS2, 4, 0); }

void _csrrw(HART* hart) {
  raise_exception_if(PRL < privilege_flag || read_only_flag, Illegal_instruction);
  reg_t temp = RS1;
  if (inst_read_flag) RD = CSRR(CSR);
  CSRW(CSR, temp);
}
void _csrrs(HART* hart) {
  raise_exception_if(PRL < privilege_flag || (inst_write_flag && read_only_flag),
                     Illegal_instruction);
  reg_t temp = RS1;
  RD         = CSRR(CSR);
  if (inst_write_flag) CSRS(CSR, temp);
}
void _csrrc(HART* hart) {
  raise_exception_if(PRL < privilege_flag || (inst_write_flag && read_only_flag),
                     Illegal_instruction);
  reg_t temp = RS1;
  RD         = CSRR(CSR);
  if (inst_write_flag) CSRC(CSR, temp);
}
void _csrrwi(HART* hart) {
  raise_exception_if(PRL < privilege_flag || read_only_flag, Illegal_instruction);
  if (inst_read_flag) RD = CSRR(CSR);
  CSRW(CSR, BITS(hart->decoder.rs1, 4, 0));
}
void _csrrsi(HART* hart) {
  raise_exception_if(PRL < privilege_flag || (inst_write_flag && read_only_flag),
                     Illegal_instruction);
  RD = CSRR(CSR);
  if (inst_write_flag) CSRS(CSR, BITS(hart->decoder.rs1, 4, 0));
}
void _csrrci(HART* hart) {
  raise_exception_if(PRL < privilege_flag || (inst_write_flag && read_only_flag),
                     Illegal_instruction);
  RD = CSRR(CSR);
  if (inst_write_flag) CSRC(CSR, BITS(hart->decoder.rs1, 4, 0));
}

void _mret(HART* hart) {
  CSRW_F(mstatus_MIE, CSRR_F(mstatus_MPIE));
  HART_PRL_MODE mpp = CSRR_F(mstatus_MPP);
  PRL               = mpp;
  CSRW_F(mstatus_MPIE, 1);
  CSRW_F(mstatus_MPP, U);
  if (mpp != M) CSRW_F(mstatus_MPRV, 0);
  NPC = CSRR(mepc);
}
void _sret(HART* hart) {
  raise_exception_if(CSRR_F(mstatus_TSR), Illegal_instruction);
  CSRW_F(mstatus_SIE, CSRR_F(mstatus_SPIE));
  HART_PRL_MODE spp = CSRR_F(mstatus_SPP);
  PRL               = spp;
  CSRW_F(mstatus_SPIE, 1);
  CSRW_F(mstatus_SPP, U);
  if (spp != M) CSRW_F(mstatus_MPRV, 0);
  NPC = CSRR(sepc);
}
void _wfi(HART* hart) { hart->state = HART_STOP; }

void _mul(HART* hart) {
  asm volatile(
      "mulq %%rdx\n\t"
      "movq %%rax,(%2)" ::"a"(RS1),
      "d"(RS2), "r"(&RD));
}
void _mulh(HART* hart) {
  asm volatile(
      "imulq %%rdx\n\t"
      "movq  %%rdx,(%2)" ::"a"(RS1),
      "d"(RS2), "r"(&RD));
}
void _mulhsu(HART* hart) {
  asm volatile(
      "mulq %%rdx\n\t"
      "movq %%rdx,(%2)" ::"a"(RS1),
      "d"(RS2), "r"(&RD));
  if (RS1S < 0) RD = RD - RS2;
}
void _mulhu(HART* hart) {
  asm volatile(
      "mulq %%rdx\n\t"
      "movq %%rdx,(%2)" ::"a"(RS1),
      "d"(RS2), "r"(&RD));
}
void _div(HART* hart) {
  if (RS2S == 0) {
    RD = ~0ull;
  } else if (RS1S == 1ull << 63 && RS2S == -1) {
    RD = 1ull << 63;
  } else {
    RD = RS1S / RS2S;
  }
}
void _divu(HART* hart) {
  if (RS2 == 0) {
    RD = ~0ull;
  } else {
    RD = RS1 / RS2;
  }
}
void _rem(HART* hart) {
  if (RS2S == 0) {
    RD = RS1S;
  } else if (RS1S == 1ull << 63 && RS2S == -1) {
    RD = 0;
  } else {
    RD = RS1S % RS2S;
  }
}
void _remu(HART* hart) {
  if (RS2 == 0) {
    RD = RS1;
  } else {
    RD = RS1 % RS2;
  }
}
void _mulw(HART* hart) {
  asm volatile(
      "mull %%edx\n\t"
      "movslq %%eax,%%rax\n\t"
      "movq %%rax,(%2)" ::"a"(RS1),
      "d"(RS2), "r"(&RD));
}
void _divw(HART* hart) {
  int32_t rs1_32 = RS1;
  int32_t rs2_32 = RS2;
  if (rs2_32 == 0) {
    RD = ~0ull;
  } else if (rs1_32 == 1 << 31 && rs2_32 == -1) {
    RD = 1 << 31;
  } else {
    RD = SEXT(rs1_32 / rs2_32, 32);
  }
}
void _divuw(HART* hart) {
  uint32_t rs1_32 = RS1;
  uint32_t rs2_32 = RS2;
  if (rs2_32 == 0) {
    RD = ~0ull;
  } else {
    RD = SEXT(rs1_32 / rs2_32, 32);
  }
}
void _remw(HART* hart) {
  int32_t rs1_32 = RS1;
  int32_t rs2_32 = RS2;
  if (rs2_32 == 0) {
    RD = SEXT(rs1_32, 32);
  } else if (rs1_32 == 1 << 31 && rs2_32 == -1) {
    RD = 0;
  } else {
    RD = SEXT(rs1_32 % rs2_32, 32);
  }
}
void _remuw(HART* hart) {
  uint32_t rs1_32 = RS1;
  uint32_t rs2_32 = RS2;
  if (rs2_32 == 0) {
    RD = ~0ull;
  } else {
    RD = SEXT(rs1_32 % rs2_32, 32);
  }
}
