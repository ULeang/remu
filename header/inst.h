#ifndef __INST_H__
#define __INST_H__

#include "hart.h"

extern void _lui(HART* hart);
extern void _auipc(HART* hart);
extern void _jal(HART* hart);
extern void _jalr(HART* hart);
extern void _beq(HART* hart);
extern void _bne(HART* hart);
extern void _blt(HART* hart);
extern void _bge(HART* hart);
extern void _bltu(HART* hart);
extern void _bgeu(HART* hart);
extern void _lb(HART* hart);
extern void _lh(HART* hart);
extern void _lw(HART* hart);
extern void _lbu(HART* hart);
extern void _lhu(HART* hart);
extern void _sb(HART* hart);
extern void _sh(HART* hart);
extern void _sw(HART* hart);
extern void _addi(HART* hart);
extern void _slti(HART* hart);
extern void _sltiu(HART* hart);
extern void _xori(HART* hart);
extern void _ori(HART* hart);
extern void _andi(HART* hart);
extern void _slli(HART* hart);
extern void _srli(HART* hart);
extern void _srai(HART* hart);
extern void _add(HART* hart);
extern void _sub(HART* hart);
extern void _sll(HART* hart);
extern void _slt(HART* hart);
extern void _sltu(HART* hart);
extern void _xor(HART* hart);
extern void _srl(HART* hart);
extern void _sra(HART* hart);
extern void _or(HART* hart);
extern void _and(HART* hart);
extern void _fence(HART* hart);
extern void _ecall(HART* hart);
extern void _ebreak(HART* hart);

extern void _lwu(HART* hart);
extern void _ld(HART* hart);
extern void _sd(HART* hart);
extern void _addiw(HART* hart);
extern void _slliw(HART* hart);
extern void _srliw(HART* hart);
extern void _sraiw(HART* hart);
extern void _addw(HART* hart);
extern void _subw(HART* hart);
extern void _sllw(HART* hart);
extern void _srlw(HART* hart);
extern void _sraw(HART* hart);

extern void _csrrw(HART* hart);
extern void _csrrs(HART* hart);
extern void _csrrc(HART* hart);
extern void _csrrwi(HART* hart);
extern void _csrrsi(HART* hart);
extern void _csrrci(HART* hart);

extern void _mret(HART* hart);
extern void _sret(HART* hart);
extern void _wfi(HART* hart);

extern void _mul(HART* hart);
extern void _mulh(HART* hart);
extern void _mulhsu(HART* hart);
extern void _mulhu(HART* hart);
extern void _div(HART* hart);
extern void _divu(HART* hart);
extern void _rem(HART* hart);
extern void _remu(HART* hart);
extern void _mulw(HART* hart);
extern void _divw(HART* hart);
extern void _divuw(HART* hart);
extern void _remw(HART* hart);
extern void _remuw(HART* hart);

typedef enum EXCEPTION_CODE {
  /* interrupt */
  // 0:Reserved
  Supervisor_software_interrupt = 1,
  // 2:Reserved
  Machine_software_interrupt = 3,
  // 4:Reserved
  Supervisor_timer_interrupt = 5,
  // 6:Reserved
  Machine_timer_interrupt = 7,
  // 8:Reserved
  Supervisor_external_interrupt = 9,
  // 10:Reserved
  Machine_external_interrupt = 11,
  // >=16:Reserved

  /* exception */
  Instruction_address_misaligned = 0,
  Instruction_access_fault       = 1,
  Illegal_instruction            = 2,
  Breakpoint                     = 3,
  Load_address_misaligned        = 4,
  Load_access_fault              = 5,
  Store_AMO_address_misaligned   = 6,
  Store_AMO_access_fault         = 7,
  Environment_call_from_U_mode   = 8,
  Environment_call_from_S_mode   = 9,
  // 10:Reserved
  Environment_call_from_M_mode = 11,
  Instruction_page_fault       = 12,
  Load_page_fault              = 13,
  // 14:Reserved
  Store_AMO_page_fault = 15,
  // 16-23:Reserved
  // 24-31:Designated for custom use
  // 32-47:Reserved
  // 48-63:Designated for custom use
  // >=64:Reserved
} EXCEPTION_CODE;

#endif
