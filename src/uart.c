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

static int uart_thread(void* arg) {
  while (1) {
    char buffer;
    tcflush(STDIN_FILENO, TCIFLUSH);
    read(STDIN_FILENO, &buffer, 1);
    if (buffer == 034) cleanup();
    atomic_store(uart_instance.registers + RHR, buffer);
    while (atomic_load(uart_instance.registers + RHR) == buffer)
      gateway_interrupt_signal(10);
  }
}
/* static int uart_thread(void* arg) {
  printf("store f\n");
  atomic_store(uart_instance.registers + RHR, 'f');
  while (atomic_load(uart_instance.registers + RHR) == 'f')
    ;
  printf("store s\n");
  atomic_store(uart_instance.registers + RHR, 's');
  return 0;
}
*/
UART uart_instance;

void uart_init() {
  for (int i = 0; i < 8; ++i) {
    atomic_init(uart_instance.registers + i, 0);
  }
  my_termios_init();
  signal(SIGINT, cleanup);
  signal(SIGQUIT, cleanup);
  signal(SIGTERM, cleanup);
  thrd_t thrd;
  thrd_create(&thrd, uart_thread, NULL);
  thrd_detach(thrd);
}
uint8_t uart_load(UART_REG_ADDR addr) {
  switch (addr) {
  case RHR: {
    return atomic_exchange(uart_instance.registers + RHR, 0);
  }
  default: return atomic_load(uart_instance.registers + addr);
  }
}
void uart_store(UART_REG_ADDR addr, uint8_t value) {
  switch (addr) {
  case THR:
    putchar(value);
    fflush(stdout);
    break;
  default: atomic_store(uart_instance.registers + addr, value);
  }
}
