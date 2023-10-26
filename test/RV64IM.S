  .text
  .globl _start
  .set UART_BASE, 0x10000000
  .set THR, 0
  .set RHR, 0
_start:
/*
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

uart_write_test:
  la    t0,UART_BASE
  la    t1,'H'
  sb    t1,THR(t0)
  la    t1,'e'
  sb    t1,THR(t0)
  la    t1,'l'
  sb    t1,THR(t0)
  la    t1,'l'
  sb    t1,THR(t0)
  la    t1,'o'
  sb    t1,THR(t0)
  la    t1,' '
  sb    t1,THR(t0)
  la    t1,'u'
  sb    t1,THR(t0)
  la    t1,'a'
  sb    t1,THR(t0)
  la    t1,'r'
  sb    t1,THR(t0)
  la    t1,'t'
  sb    t1,THR(t0)
  la    t1,'!'
  sb    t1,THR(t0)
  la    t1,'\n'
  sb    t1,THR(t0)
  */
uart_read_test:
  la    t0,UART_BASE
  lb    t1,RHR(t0)
  beqz  t1,uart_read_test
  sb    t1,THR(t0)
  j     uart_read_test
  wfi

/*
res:
  .8byte 0
data1:
  .8byte 0x1919810
helloworld:
  .asciz  "Hello World!"
  */
