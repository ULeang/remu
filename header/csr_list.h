#ifndef __CSR_LIST_H__
#define __CSR_LIST_H__

typedef enum CSR_ADDR {
  /* Unprivileged Floating-Point CSRs */
  fflags = 0x001,
  frm,
  fcsr,
  /* Unprivileged Counter/Timers */
  cycle = 0xc00,
  // time, redefinition, so change to csr_time
  csr_time,
  instret,
  hpmcounter3,
  hpmcounter4,
  hpmcounter5,
  hpmcounter6,
  hpmcounter7,
  hpmcounter8,
  hpmcounter9,
  hpmcounter10,
  hpmcounter11,
  hpmcounter12,
  hpmcounter13,
  hpmcounter14,
  hpmcounter15,
  hpmcounter16,
  hpmcounter17,
  hpmcounter18,
  hpmcounter19,
  hpmcounter20,
  hpmcounter21,
  hpmcounter22,
  hpmcounter23,
  hpmcounter24,
  hpmcounter25,
  hpmcounter26,
  hpmcounter27,
  hpmcounter28,
  hpmcounter29,
  hpmcounter30,
  hpmcounter31,

  /* Supervisor Trap Setup */
  sstatus = 0x100,
  sie     = 0x104,
  stvec,
  scounteren,
  /* Supervisor Configuration */
  senvcfg = 0x10a,
  /* Supervisor Trap Handling */
  sscratch = 0x140,
  sepc,
  scause,
  stval,
  sip,
  /* Supervisor Protection and Translation */
  satp = 0x180,
  /* Debug/Trace Registers */
  scontext = 0x5a8,

  /* Hypervisor Trap Setup */
  hstatus = 0x600,
  hedeleg = 0x602,
  hideleg,
  hie,
  hcounteren = 0x606,
  hgeie,
  /* Hypervisor Trap Handling */
  htval = 0x643,
  hip,
  hvip,
  htinst = 0x64a,
  hgeip  = 0xe12,
  /* Hypervisor Configuration */
  henvcfg = 0x60a,
  /* Hypervisor Protection and Translation */
  hgatp = 0x680,
  /* Debug/Trace Registers */
  hcontext = 0x6a8,
  /* Hypervisor Counter/Timer Virtualization Registers */
  htimedelta = 0x605,
  /* Virtual Supervisor Registers */
  vsstatus = 0x200,
  vsie     = 0x204,
  vstvec,
  vsscratch = 0x240,
  vsepc,
  vscause,
  vstval,
  vsip,
  vsatp = 0x280,

  /* Machine Information Registers */
  mvendorid = 0xf11,
  marchid,
  mimpid,
  mhartid,
  mconfigptr,
  /* machine trap setup */
  mstatus = 0x300,
  misa,
  medeleg,
  mideleg,
  mie,
  mtvec,
  mcounteren,
  /* Machine Trap Handling */
  mscratch = 0x340,
  mepc,
  mcause,
  mtval,
  mip,
  mtinst = 0x34a,
  mtval2,
  /* Machine Configuration */
  menvcfg = 0x30a,
  mseccfg = 0x747,
  /* Machine Memory Protection */
  pmpcfg0  = 0x3a0,
  pmpcfg2  = 0x3a2,
  pmpcfg4  = 0x3a4,
  pmpcfg6  = 0x3a6,
  pmpcfg8  = 0x3a8,
  pmpcfg10 = 0x3aa,
  pmpcfg12 = 0x3ac,
  pmpcfg14 = 0x3ae,
  pmpaddr0 = 0x3b0,
  pmpaddr1,
  pmpaddr2,
  pmpaddr3,
  pmpaddr4,
  pmpaddr5,
  pmpaddr6,
  pmpaddr7,
  pmpaddr8,
  pmpaddr9,
  pmpaddr10,
  pmpaddr11,
  pmpaddr12,
  pmpaddr13,
  pmpaddr14,
  pmpaddr15,
  pmpaddr16,
  pmpaddr17,
  pmpaddr18,
  pmpaddr19,
  pmpaddr20,
  pmpaddr21,
  pmpaddr22,
  pmpaddr23,
  pmpaddr24,
  pmpaddr25,
  pmpaddr26,
  pmpaddr27,
  pmpaddr28,
  pmpaddr29,
  pmpaddr30,
  pmpaddr31,
  pmpaddr32,
  pmpaddr33,
  pmpaddr34,
  pmpaddr35,
  pmpaddr36,
  pmpaddr37,
  pmpaddr38,
  pmpaddr39,
  pmpaddr40,
  pmpaddr41,
  pmpaddr42,
  pmpaddr43,
  pmpaddr44,
  pmpaddr45,
  pmpaddr46,
  pmpaddr47,
  pmpaddr48,
  pmpaddr49,
  pmpaddr50,
  pmpaddr51,
  pmpaddr52,
  pmpaddr53,
  pmpaddr54,
  pmpaddr55,
  pmpaddr56,
  pmpaddr57,
  pmpaddr58,
  pmpaddr59,
  pmpaddr60,
  pmpaddr61,
  pmpaddr62,
  pmpaddr63,
  /* Machine Counter/Timers */
  mcycle   = 0xb00,
  minstret = 0xb02,
  mhpmcounter3,
  mhpmcounter4,
  mhpmcounter5,
  mhpmcounter6,
  mhpmcounter7,
  mhpmcounter8,
  mhpmcounter9,
  mhpmcounter10,
  mhpmcounter11,
  mhpmcounter12,
  mhpmcounter13,
  mhpmcounter14,
  mhpmcounter15,
  mhpmcounter16,
  mhpmcounter17,
  mhpmcounter18,
  mhpmcounter19,
  mhpmcounter20,
  mhpmcounter21,
  mhpmcounter22,
  mhpmcounter23,
  mhpmcounter24,
  mhpmcounter25,
  mhpmcounter26,
  mhpmcounter27,
  mhpmcounter28,
  mhpmcounter29,
  mhpmcounter30,
  mhpmcounter31,
  /* Machine Counter Setup */
  mcountinhibit = 0x320,
  mhpmevent3    = 0x323,
  mhpmevent4,
  mhpmevent5,
  mhpmevent6,
  mhpmevent7,
  mhpmevent8,
  mhpmevent9,
  mhpmevent10,
  mhpmevent11,
  mhpmevent12,
  mhpmevent13,
  mhpmevent14,
  mhpmevent15,
  mhpmevent16,
  mhpmevent17,
  mhpmevent18,
  mhpmevent19,
  mhpmevent20,
  mhpmevent21,
  mhpmevent22,
  mhpmevent23,
  mhpmevent24,
  mhpmevent25,
  mhpmevent26,
  mhpmevent27,
  mhpmevent28,
  mhpmevent29,
  mhpmevent30,
  mhpmevent31,
  /* Debug/Trace Registers(shared with Debug Mode) */
  tselect = 0x7a0,
  tdata1,
  tdata2,
  tdata3,
  mcontext = 0x7a8,
  /* Debug Mode Registers */
  dcsr = 0x7b0,
  dpc,
  dscratch0,
  dscratch1,
} CSR_ADDR;

#endif
