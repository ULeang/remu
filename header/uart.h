#ifndef __UART_H__
#define __UART_H__

#include "plic.h"
#include "type.h"

typedef struct ns16550 {
  uint8_t registers[8];
} UART;

typedef enum UART_REG_ADDR {
  RHR = 0b000,  // Receive Holding Register
  THR = 0b000,  // Transmit Holding Register
  IER = 0b001,  // Interrupt Enable Register
  FCR = 0b010,  // FIFO control Register
  ISR = 0b010,  // Interrupt Status Register
  LCR = 0b011,  // Line Control Register
  MCR = 0b100,  // Modem Control Register
  LSR = 0b101,  // Line Status Register
  MSR = 0b110,  // Modem Status Register
  SPR = 0b111,  // Scratchpad Register Read
} UART_REG_ADDR;

extern UART uart_instance;

extern void    uart_init();
extern uint8_t uart_load(UART* uart, UART_REG_ADDR addr);
extern void    uart_store(UART* uart, UART_REG_ADDR addr, uint8_t value);

#endif
