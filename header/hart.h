#ifndef __HART_H__
#define __HART_H__

#include <stdatomic.h>

#include "bus.h"
#include "csr.h"
#include "type.h"

typedef enum INST_NAME {
  // RV64I
  LUI,
  AUIPC,
  JAL,
  JALR,
  BEQ,
  BNE,
  BLT,
  BGE,
  BLTU,
  BGEU,
  LB,
  LH,
  LW,
  LBU,
  LHU,
  SB,
  SH,
  SW,
  ADDI,
  SLTI,
  SLTIU,
  XORI,
  ORI,
  ANDI,
  SLLI,
  SRLI,
  SRAI,
  ADD,
  SUB,
  SLL,
  SLT,
  SLTU,
  XOR,
  SRL,
  SRA,
  OR,
  AND,
  FENCE,
  ECALL,
  EBREAK,
  LWU,
  LD,
  SD,
  ADDIW,
  SLLIW,
  SRLIW,
  SRAIW,
  ADDW,
  SUBW,
  SLLW,
  SRLW,
  SRAW,
  // Zicsr
  CSRRW,
  CSRRS,
  CSRRC,
  CSRRWI,
  CSRRSI,
  CSRRCI,

  MRET,
  SRET,
  WFI,

  // M
  MUL,
  MULH,
  MULHSU,
  MULHU,
  DIV,
  DIVU,
  REM,
  REMU,
  MULW,
  DIVW,
  DIVUW,
  REMW,
  REMUW,
  // end
  INST_NUM,
} INST_NAME;

typedef enum REG_ABINAME {
  zero = 0,
  ra,
  sp,
  gp,
  tp,
  t0,
  t1,
  t2,
  s0,
  s1,
  a0,
  a1,
  a2,
  a3,
  a4,
  a5,
  a6,
  a7,
  s2,
  s3,
  s4,
  s5,
  s6,
  s7,
  s8,
  s9,
  s10,
  s11,
  t3,
  t4,
  t5,
  t6
} REG_ABINAME;

extern const char* reg_abinames[];

typedef enum HART_STATE {
  HART_STOP,
  HART_RUN,
} HART_STATE;

typedef enum HART_PRL_MODE {
  U = 0b00,
  S = 0b01,
  M = 0b11,
} HART_PRL_MODE;

typedef struct DECODER {
  uint32_t    inst;
  INST_NAME   inst_name;
  REG_ABINAME rd;
  REG_ABINAME rs1;
  REG_ABINAME rs2;
  CSR_ADDR    csr_addr;
  reg_t       imm;
  uint8_t     shamt;
  // pc of this instruction
  reg_t cpc;
  // pc of next instruction, inst like JAL should modify npc instead of hart's pc
  reg_t npc;
} DECODER;

typedef struct HART {
  reg_t         gpr[32];
  reg_t         pc;
  BUS*          bus;
  HART_STATE    state;
  HART_PRL_MODE privilege;
  CSR           csr;
  DECODER       decoder;
} HART;

extern void (*inst_handler[INST_NUM])(HART*);
extern HART* mk_hart(BUS* bus);
extern void  hart_reset(HART* hart);
extern void  hart_run(HART* hart);
extern void  hart_step(HART* hart);

extern void fetch(HART* hart);
extern void decode(HART* hart);

#endif
