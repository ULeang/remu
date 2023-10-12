#include "hart.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "inst.h"
#include "utility.h"

static reg_t imm_i(uint32_t inst) { return SEXT(BITS(inst, 31, 20), 12); }
static reg_t imm_s(uint32_t inst) {
  return SEXT((BITS(inst, 31, 25) << 5) | BITS(inst, 11, 7), 12);
}
static reg_t imm_b(uint32_t inst) {
  return SEXT((BITS(inst, 31, 31) << 12) | (BITS(inst, 7, 7) << 11) |
                  (BITS(inst, 30, 25) << 5) | (BITS(inst, 11, 8) << 1),
              13);
}
static reg_t imm_u(uint32_t inst) { return SEXT(inst & (MASK(20) << 12), 32); }
static reg_t imm_j(uint32_t inst) {
  return SEXT((BITS(inst, 31, 31) << 20) | (BITS(inst, 19, 12) << 12) |
                  (BITS(inst, 20, 20) << 11) | (BITS(inst, 30, 21) << 1),
              21);
}

static uint32_t inst_fetch(HART *hart) { return mem_load(hart->bus, hart->pc, 4); }
static void     decode(HART *hart) {
  DECODER *dec  = &hart->decoder;
  uint32_t inst = inst_fetch(hart);

  dec->inst     = inst;
  dec->rd       = BITS(inst, 11, 7);
  dec->rs1      = BITS(inst, 19, 15);
  dec->rs2      = BITS(inst, 24, 20);
  dec->csr_addr = BITS(inst, 31, 20);
  dec->cpc      = hart->pc;
  dec->npc      = hart->pc + 4;

  uint32_t funct3 = BITS(inst, 14, 12);
  uint32_t funct7 = BITS(inst, 31, 25);
  uint32_t funct6 = BITS(inst, 31, 26);
  uint32_t opcode = BITS(inst, 6, 0);
  switch (opcode) {
  case 0b0110111:
    dec->inst_name = LUI;
    dec->imm       = imm_u(inst);
    break;
  case 0b0010111:
    dec->inst_name = AUIPC;
    dec->imm       = imm_u(inst);
    break;
  case 0b1101111:
    dec->inst_name = JAL;
    dec->imm       = imm_j(inst);
    break;
  case 0b1100111:
    dec->inst_name = JALR;
    dec->imm       = imm_i(inst);
    break;
  case 0b1100011:
    dec->imm = imm_b(inst);
    switch (funct3) {
    case 0b000: dec->inst_name = BEQ; break;
    case 0b001: dec->inst_name = BNE; break;
    case 0b100: dec->inst_name = BLT; break;
    case 0b101: dec->inst_name = BGE; break;
    case 0b110: dec->inst_name = BLTU; break;
    case 0b111: dec->inst_name = BGEU; break;
    }
    break;
  case 0b0000011:
    dec->imm = imm_i(inst);
    switch (funct3) {
    case 0b000: dec->inst_name = LB; break;
    case 0b001: dec->inst_name = LH; break;
    case 0b010: dec->inst_name = LW; break;
    case 0b011: dec->inst_name = LD; break;
    case 0b100: dec->inst_name = LBU; break;
    case 0b101: dec->inst_name = LHU; break;
    case 0b110: dec->inst_name = LWU; break;
    }
    break;
  case 0b0100011:
    dec->imm = imm_s(inst);
    switch (funct3) {
    case 0b000: dec->inst_name = SB; break;
    case 0b001: dec->inst_name = SH; break;
    case 0b010: dec->inst_name = SW; break;
    case 0b011: dec->inst_name = SD; break;
    }
    break;
  case 0b0010011:
    dec->imm   = imm_i(inst);
    dec->shamt = BITS(inst, 25, 20);
    switch (funct3) {
    case 0b000: dec->inst_name = ADDI; break;
    case 0b010: dec->inst_name = SLTI; break;
    case 0b011: dec->inst_name = SLTIU; break;
    case 0b100: dec->inst_name = XORI; break;
    case 0b110: dec->inst_name = ORI; break;
    case 0b111: dec->inst_name = ANDI; break;
    case 0b001: dec->inst_name = SLLI; break;
    case 0b101:
      switch (funct6) {
      case 0b000000: dec->inst_name = SRLI; break;
      case 0b010000: dec->inst_name = SRAI; break;
      }
      break;
    }
    break;
  case 0b0011011:
    dec->imm   = imm_i(inst);
    dec->shamt = BITS(inst, 24, 20);
    switch (funct3) {
    case 0b000: dec->inst_name = ADDIW; break;
    case 0b001: dec->inst_name = SLLIW; break;
    case 0b101:
      switch (funct7) {
      case 0b0000000: dec->inst_name = SRLIW; break;
      case 0b0100000: dec->inst_name = SRAIW; break;
      }
      break;
    }
    break;
  case 0b0110011:
    switch ((funct7 << 3) | funct3) {
    case 0b0000000000: dec->inst_name = ADD; break;
    case 0b0100000000: dec->inst_name = SUB; break;
    case 0b001: dec->inst_name = SLL; break;
    case 0b010: dec->inst_name = SLT; break;
    case 0b011: dec->inst_name = SLTU; break;
    case 0b100: dec->inst_name = XOR; break;
    case 0b0000000101: dec->inst_name = SRL; break;
    case 0b0100000101: dec->inst_name = SRA; break;
    case 0b110: dec->inst_name = OR; break;
    case 0b111: dec->inst_name = AND; break;
    }
    break;
  case 0b0111011:
    switch ((funct7 << 3) | funct3) {
    case 0b0000000000: dec->inst_name = ADDW; break;
    case 0b0100000000: dec->inst_name = SUBW; break;
    case 0b001: dec->inst_name = SLLW; break;
    case 0b0000000101: dec->inst_name = SRLW; break;
    case 0b0100000101: dec->inst_name = SRAW; break;
    }
    break;
  case 0b0001111:
    switch (funct3) {
    case 0b000: dec->inst_name = FENCE; break;
    case 0b001: dec->inst_name = FENCE; break;
    }
    break;
  case 0b1110011:
    switch (funct3) {
    case 0b000:
      switch ((funct7) << 5 | dec->rs2) {
      case 0b000000000000: dec->inst_name = ECALL;
      case 0b000000000001: dec->inst_name = EBREAK;
      case 0b000100000010: dec->inst_name = SRET;
      case 0b001100000010: dec->inst_name = MRET;
      case 0b000100000101: dec->inst_name = WFI;
      }
      break;
    case 0b001: dec->inst_name = CSRRW; break;
    case 0b010: dec->inst_name = CSRRS; break;
    case 0b011: dec->inst_name = CSRRC; break;
    case 0b101: dec->inst_name = CSRRWI; break;
    case 0b110: dec->inst_name = CSRRSI; break;
    case 0b111: dec->inst_name = CSRRCI; break;
    }
    break;
  default: dec->inst_name = INST_NUM; break;
  }
}

HART *mk_hart(BUS *bus) {
  HART *ret = malloc(sizeof(HART));
  assert(ret);
  ret->bus = bus;
  return ret;
}
void hart_reset(HART *hart) {
  hart->pc = DRAM_BASE;
  memset(hart->gpr, 0, sizeof(hart->gpr));
  hart->state     = HART_RUN;
  hart->privilege = M;
  csr_init(&hart->csr);
  // 10
  // 00 0000 0000 0000 0000 0000 0000 0000 0000 00
  // 00 0001 0100 0000 0001 0000 0000
  csrw(&hart->csr, misa, 0x8000000000140100);
  csrw(&hart->csr, mhartid, 0x0);
  // 0'000 0000 0000 0000 0000 0000 00'0'0' 10'10' 0000 0000 0'
  // 0'0'0' 0'0'0'0 0'00'0 0'00'0' 0'0'0'0' 0'0'0'0
  csrw(&hart->csr, mstatus, 0x0000000a00000000);
}
void hart_run(HART *hart) {
  while (hart->state == HART_RUN) {
    hart_step(hart);
  }
}
void hart_step(HART *hart) {
  decode(hart);
  if (hart->decoder.inst_name == INST_NUM) {
    hart->state = HART_STOP;
    return;
  }
  inst_handler[hart->decoder.inst_name](hart);
  hart->gpr[0] = 0;
  hart->pc     = hart->decoder.npc;
}

static const char *reg_abinames[] = {
    "zero", "ra", "sp", "gp", "tp",  "t0",  "t1", "t2", "s0", "s1", "a0",
    "a1",   "a2", "a3", "a4", "a5",  "a6",  "a7", "s2", "s3", "s4", "s5",
    "s6",   "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

void print_next_disassmembly(HART *hart) {
  decode(hart);
  printf("-> %p : %08x\n", (void *)hart->pc, hart->decoder.inst);
}
void print_gpr(HART *hart) {
  for (int i = 0; i < 16; ++i)
    printf("x%d(%s)\t: 0x%-8lx = %-ld\t\tx%d(%s)\t: 0x%-8lx = %ld\n", i,
           reg_abinames[i], hart->gpr[i], hart->gpr[i], i + 16, reg_abinames[i + 16],
           hart->gpr[i + 16], hart->gpr[i + 16]);
}

void (*inst_handler[INST_NUM])(HART *) = {
    [LUI] = _lui,       [AUIPC] = _auipc,   [JAL] = _jal,     [JALR] = _jalr,
    [BEQ] = _beq,       [BNE] = _bne,       [BLT] = _blt,     [BGE] = _bge,
    [BLTU] = _bltu,     [BGEU] = _bgeu,     [LB] = _lb,       [LH] = _lh,
    [LW] = _lw,         [LBU] = _lbu,       [LHU] = _lhu,     [SB] = _sb,
    [SH] = _sh,         [SW] = _sw,         [ADDI] = _addi,   [SLTI] = _slti,
    [SLTIU] = _sltiu,   [XORI] = _xori,     [ORI] = _ori,     [ANDI] = _andi,
    [SLLI] = _slli,     [SRLI] = _srli,     [SRAI] = _srai,   [ADD] = _add,
    [SUB] = _sub,       [SLL] = _sll,       [SLT] = _slt,     [SLTU] = _sltu,
    [XOR] = _xor,       [SRL] = _srl,       [SRA] = _sra,     [OR] = _or,
    [AND] = _and,       [FENCE] = _fence,   [ECALL] = _ecall, [EBREAK] = _ebreak,
    [LWU] = _lwu,       [LD] = _ld,         [SD] = _sd,       [ADDIW] = _addiw,
    [SLLIW] = _slliw,   [SRLIW] = _srliw,   [SRAIW] = _sraiw, [ADDW] = _addw,
    [SUBW] = _subw,     [SLLW] = _sllw,     [SRLW] = _srlw,   [SRAW] = _sraw,
    [CSRRW] = _csrrw,   [CSRRS] = _csrrs,   [CSRRC] = _csrrc, [CSRRWI] = _csrrwi,
    [CSRRSI] = _csrrsi, [CSRRCI] = _csrrci, [MRET] = _mret,   [SRET] = _sret,
    [WFI] = _wfi,
};
