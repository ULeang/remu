#include "uart.h"

#include <signal.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <threads.h>
#include <unistd.h>

static struct termios ti_backup;

static int my_termios_init() {
  struct termios ti;
  tcgetattr(STDIN_FILENO, &ti_backup);
  ti             = ti_backup;
  ti.c_cc[VTIME] = 0;
  ti.c_cc[VMIN]  = 1;
  ti.c_lflag &= ~(ISIG | ICANON | IEXTEN | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &ti);
  return 0;
}
static int my_termios_recover() {
  return tcsetattr(STDIN_FILENO, TCSANOW, &ti_backup);
}
static void cleanup() {
  printf("quiting...\n");
  my_termios_recover();
  exit(1);
}

static int uart_thread(void* arg) { return 0; }

UART uart_instance = {.registers[RHR] = 0, .registers[LSR] = 0};

void uart_prepare() {
  my_termios_init();
  signal(SIGINT, cleanup);
  signal(SIGQUIT, cleanup);
  signal(SIGTERM, cleanup);
  thrd_t thrd;
  thrd_create(&thrd, uart_thread, NULL);
  thrd_detach(thrd);
}
uint8_t uart_load(UART_REG_ADDR addr) { return uart_instance.registers[addr]; }
void    uart_store(UART_REG_ADDR addr, uint8_t value) {
  uart_instance.registers[addr] = value;
}
