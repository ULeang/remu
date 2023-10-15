  .text
  .globl _start
_start:
  li    t0,1145141919
  li    t1,514
  mul   t0,t0,t1
  mul   a0,t0,t0
  mulhu a1,t0,t0
  la    a3,res
  sd    a0,(a3)
  la    a3,data1
  ld    a4,(a3)
  div   a5,a4,t1
  rem   a6,a4,t1
  wfi

res:
  .8byte 0
data1:
  .8byte 0x1919810
helloworld:
  .asciz  "Hello World!"
