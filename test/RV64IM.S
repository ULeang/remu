  .text
  .set  plic_base,  0xc000000
  .set  claim,  0x200004
  .set  uart_priority_addr, 10*4
  .set  IE, 0x2000
  .set  UART_BASE,  0x10000000
  .set  THR, 0
  .set  RHR, 0
  .globl _start
_start:
  la    t0,M_trap_vector
  csrw  mtvec,t0
  li    t0,0x8
  csrrs x0,mstatus,t0
  li    t0,0x0800
  csrrs x0,mie,t0
  li    t0,plic_base
  li    t1,7
  sw    t1,uart_priority_addr(t0)
  li    t2,IE
  add   t0,t0,t2
  li    t1,0xffffffff
  sw    t1,(t0)
loop:
  j     loop

  .balign 4
M_trap_vector:
  csrr  t0,mcause
  li    t1,0x800000000000000b
  bne   t0,t1,exit
  li    t0,plic_base
  li    t1,claim
  add   t2,t0,t1
  lw    t3,(t2)
  li    t4,10
  bne   t3,t4,exit
  li    t5,UART_BASE
  lb    t0,RHR(t5)
  sb    t0,THR(t5)
  sw    t3,(t2)
exit:
  mret
