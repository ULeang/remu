#include "monitor.h"

#include <ctype.h>
#include <readline/chardefs.h>
#include <stdlib.h>
#include <string.h>

// clang-format off
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
// clang-format on

#define isfirstcomchar(c) (isalpha(c) || (c) == '_')
#define iscomchar(c)      (isalnum(c) || (c) == '_')
#define max(a, b)         ((a) >= (b) ? (a) : (b))
#define min(a, b)         ((a) <= (b) ? (a) : (b))

// input: string end with '\0', not including NULL
// strip white char leading or trailing the input string
static char *stripwhite(char *string) {
  char *s, *t;
  for (s = string; whitespace(*s); ++s)
    ;
  if (*s == 0) return s;
  t = s + strlen(s) - 1;
  while (t > s && whitespace(*t)) --t;
  *++t = '\0';
  return s;
}
// when called, free the string allocated by malloc before so dont free it manually,
// use strdup to keep a copy if it is needed by further use
char *rl() {
  static char *input = NULL;
  static char *s     = NULL;
  free(input);
  input = readline("~>");
  if (input) {
    s = stripwhite(input);
    if (*s) {
      add_history(s);
    }
    return s;
  } else
    return input;
}

typedef struct {
  char *regname;
  enum { _GPR, _CSR } type;
  int index;
} REGS;

static const REGS regs[] = {
    {"zero", _GPR, 0},
    {"ra", _GPR, ra},
    {"sp", _GPR, sp},
    {"gp", _GPR, gp},
    {"tp", _GPR, tp},
    {"t0", _GPR, t0},
    {"t1", _GPR, t1},
    {"t2", _GPR, t2},
    {"s0", _GPR, s0},
    {"fp", _GPR, s0},
    {"s1", _GPR, s1},
    {"a0", _GPR, a0},
    {"a1", _GPR, a1},
    {"a2", _GPR, a2},
    {"a3", _GPR, a3},
    {"a4", _GPR, a4},
    {"a5", _GPR, a5},
    {"a6", _GPR, a6},
    {"a7", _GPR, a7},
    {"s2", _GPR, s2},
    {"s3", _GPR, s3},
    {"s4", _GPR, s4},
    {"s5", _GPR, s5},
    {"s6", _GPR, s6},
    {"s7", _GPR, s7},
    {"s8", _GPR, s8},
    {"s9", _GPR, s9},
    {"s10", _GPR, s10},
    {"s11", _GPR, s11},
    {"t3", _GPR, t3},
    {"t4", _GPR, t4},
    {"t5", _GPR, t5},
    {"t6", _GPR, t6},
    {"sstatus", _CSR, sstatus},
    {"sie", _CSR, sie},
    {"stvec", _CSR, stvec},
    {"sscratch", _CSR, sscratch},
    {"sepc", _CSR, sepc},
    {"scause", _CSR, scause},
    {"stval", _CSR, stval},
    {"sip", _CSR, sip},
    {"mhartid", _CSR, mhartid},
    {"mstatus", _CSR, mstatus},
    {"misa", _CSR, misa},
    {"medeleg", _CSR, medeleg},
    {"mideleg", _CSR, mideleg},
    {"mie", _CSR, mie},
    {"mtvec", _CSR, mtvec},
    {"mscratch", _CSR, mscratch},
    {"mepc", _CSR, mepc},
    {"mcause", _CSR, mcause},
    {"mtval", _CSR, mtval},
    {"mip", _CSR, mip},
    {NULL, _GPR, 0},
};

static int find_reg(char **reg) {
  char *tocmp = NULL;
  for (int i = 0; (tocmp = regs[i].regname); ++i) {
    if (strncmp(*reg, tocmp, strlen(tocmp)) == 0) {
      *reg += strlen(tocmp);
      return i;
    }
  }
  return -1;
}

static int com_registers(MACHINE *machine, char *args);
static int com_memory(MACHINE *machine, char *args);
static int com_reset(MACHINE *machine, char *args);
static int com_step(MACHINE *machine, char *args);
static int com_continue(MACHINE *machine, char *args);
static int com_display(MACHINE *machine, char *args);
static int com_watch(MACHINE *machine, char *args);
static int com_breakpoint(MACHINE *machine, char *args);
static int com_help(MACHINE *machine, char *args);
static int com_inst(MACHINE *machine, char *args);
static int com_quit(MACHINE *machine, char *args);
static int com_eval(MACHINE *machine, char *args);

typedef int com_t(MACHINE *, char *);
typedef struct {
  char  *name;
  char  *abbr;
  com_t *func;
  char  *doc;
} COMMAND;

static const COMMAND commands[] = {
    {"register", "reg", com_registers, "List registers"},
    {"memory", "x", com_memory, "Examine memory"},
    {"reset", "r", com_reset, "Reset machine"},
    {"step", "s", com_step, "Execute one inst"},
    {"continue", "c", com_continue, "Run machine"},
    {"display", "d", com_display, "Auto print value"},
    {"watch", "w", com_watch, "Set watchpoints"},
    {"break", "b", com_breakpoint, "Set breakpoints"},
    {"help", "h", com_help, "Show help"},
    {"inst", "l", com_inst, "Show current inst"},
    {"quit", "q", com_quit, "Quit remu"},
    {"eval", "e", com_eval, "Evaluate expression"},
    {NULL, NULL, NULL, NULL},
};

#define rprintf(name, fmt, args...) printf("\033[;36m%s\033[m -> " fmt, name, ##args)
#define mprintf(addr, fmt, args...) \
  printf("\033[;31m%p\033[m -> " fmt, (void *)(addr), ##args)

static int com_registers(MACHINE *machine, char *reg) {
  int display_all_flag = 1;
  while (*reg) {
    if (whitespace(*reg)) {
      ++reg;
      continue;
    }
    int list_index = find_reg(&reg);
    if (list_index >= 0) {
      reg_t v = regs[list_index].type == _CSR
                    ? csrr(&machine->hart->csr, regs[list_index].index)
                    : machine->hart->gpr[regs[list_index].index];
      rprintf(regs[list_index].regname, "0x%lx = %lu\n", v, v);
      display_all_flag = 0;
    } else {
      printf("error: unrecognized register name\n");
      return 1;
    }
  }
  if (display_all_flag) {
    for (int i = 0; i < 16; ++i) {
      reg_t v   = machine->hart->gpr[i];
      reg_t v_2 = machine->hart->gpr[i + 16];
      rprintf(reg_abinames[i], "0x%lx = %lu\t\t", v, v);
      rprintf(reg_abinames[i + 16], "0x%lx = %lu\n", v_2, v_2);
    }
  }
  return 0;
}
static int com_memory(MACHINE *machine, char *fmt_addr) {
  unsigned long count                                    = 1;
  static enum { O, X, D, U, T, F, A, I, C, S, Z } format = U;
  static enum { B = 1, H = 2, W = 4, G = 8 } size        = B;
  static reg_t addr                                      = 0x0;

  char *s = stripwhite(fmt_addr);
  if (s[0] == '/') {
    ++s;
    if (isdigit(*s)) {
      unsigned long temp = strtoul(s, &s, 10);
      if (temp == 0) {
        printf("invalid count\n");
        return -1;
      }
      count = min(1024, temp);
    }
    while (!whitespace(*s) && *s != '\0') {
      switch (*s) {
      case 'o':
        format = O;
        ++s;
        break;
      case 'x':
        format = X;
        ++s;
        break;
      case 'd':
        format = D;
        ++s;
        break;
      case 'u':
        format = U;
        ++s;
        break;
      case 't':
        format = T;
        ++s;
        break;
      case 'f':
        format = F;
        ++s;
        break;
      case 'a':
        format = A;
        ++s;
        break;
      case 'i':
        format = I;
        ++s;
        break;
      case 'c':
        format = C;
        ++s;
        break;
      case 's':
        format = S;
        ++s;
        break;
      case 'z':
        format = Z;
        ++s;
        break;
      case 'b':
        size = B;
        ++s;
        break;
      case 'h':
        size = H;
        ++s;
        break;
      case 'w':
        size = W;
        ++s;
        break;
      case 'g':
        size = G;
        ++s;
        break;
      default: printf("unrecognized format or size letter '%c'\n", *s); return -2;
      }
    }
  }
  s = stripwhite(s);
  if (s[0] != '\0') {
    unsigned long temp = strtoul(s, &s, 16);
    addr               = temp;
    s                  = stripwhite(s);
  }
  if (addr == 0) {
    printf("address needed\n");
    return -3;
  }
  if (s[0] != '\0') {
    printf("to many args '%s'\n", s);
    return -4;
  }

  if (format == I)
    size = W;
  else if (format == A)
    size = G;
  else if (format == C)
    size = B;
  else if (format == S)
    size = B;

  int i = 0;
  while (i < count) {
    int nprint = min(16 / size, count - i);
    if (format == S) nprint = 1;
    mprintf(addr, "");
    for (int ii = 0; ii < nprint; ++ii) {
      switch (format) {
      case O: printf(&" %lo"[(ii == 0)], mem_load(addr, size)); break;
      case X: printf(&" %lx"[(ii == 0)], mem_load(addr, size)); break;
      case D: printf(&" %ld"[(ii == 0)], mem_load(addr, size)); break;
      case U: printf(&" %lu"[(ii == 0)], mem_load(addr, size)); break;
      case T: printf(&" %lx"[(ii == 0)], mem_load(addr, size)); break;
      case F: printf(&" %f"[(ii == 0)], mem_load(addr, size)); break;
      case A: printf(&" %p"[(ii == 0)], mem_load(addr, size)); break;
      case I: printf(&" %08lx"[(ii == 0)], mem_load(addr, size)); break;
      case C: {
        unsigned char c = mem_load(addr, size);
        if (isprint(c))
          printf(&" %c"[ii == 0], c);
        else {
          printf(&" '\\%03o'"[ii == 0], c);
        }
        break;
      }
      case S:
        printf(&" %s"[(ii == 0)],
               machine -> hart -> bus -> dram -> dram + (addr - DRAM_BASE));
        addr += strlen((char *)machine->hart->bus->dram->dram + (addr - DRAM_BASE));
        break;
      case Z: printf(&" %0*lx"[(ii == 0)], size * 2, mem_load(addr, size)); break;
      }
      addr += size;
    }
    printf("\n");
    i += nprint;
  }
  return 0;
}
static int com_reset(MACHINE *machine, char *_must_no_args) {
  printf("reset\n");
  hart_reset(machine->hart);
  com_inst(machine, "");
  return 0;
}
static int com_step(MACHINE *machine, char *count) {
  printf("step\n");
  hart_step(machine->hart);
  com_inst(machine, "");
  return 0;
}
static int com_continue(MACHINE *machine, char *_must_no_args) {
  printf("continue\n");
  hart_run(machine->hart);
  com_inst(machine, "");
  return 0;
}
static int com_display(MACHINE *machine, char *fmt_addr_of_regs) {}
static int com_watch(MACHINE *machine, char *regs) {}
static int com_breakpoint(MACHINE *machine, char *addr) {}
static int com_help(MACHINE *machine, char *command) {
  for (int i = 0; commands[i].name; ++i) {
    printf("%-10s - %-3s\t--\t%s\n", commands[i].name, commands[i].abbr,
           commands[i].doc);
  }
  return 0;
}

static int com_inst(MACHINE *machine, char *_must_no_args) {
  fetch(machine->hart);
  decode(machine->hart);
  printf("\033[;36mpc\033[m = %p -> %08x\n", (void *)machine->hart->decoder.cpc,
         machine->hart->decoder.inst);
  return 0;
}
static int com_quit(MACHINE *machine, char *_must_no_args) {
  char *args = stripwhite(_must_no_args);
  if (*args) {
    printf("invalid args for quit\n");
    return -1;
  }

  char *input;
  char *s;
  while (1) {
    input = readline("Quitting remu (y/n): ");
    if (!input) {
      printf("EOF [assumed Y]\n");
      exit(0);
    }
    s = stripwhite(input);
    if (!*s) {
      free(input);
      continue;
    }
    if (strncmp(s, "y", strlen(s)) == 0 ||
        strncmp(s, "yes", max(strlen(s), 3)) == 0) {
      free(input);
      exit(0);
    } else if (strncmp(s, "n", strlen(s)) == 0 ||
               strncmp(s, "no", max(strlen(s), 2)) == 0) {
      free(input);
      return 0;
    } else {
      free(input);
      printf("not confirmed\n");
      return 0;
    }
  }
}
static int com_eval(MACHINE *machine, char *expr) {}

// typedef char *rl_compentry_func_t (const char *, int);
static char *command_generator(const char *text, int state) {
  static int list_index, len;
  char      *name;
  if (state == 0) {
    list_index = 0;
    len        = strlen(text);
  }
  while ((name = commands[list_index].name)) {
    ++list_index;
    if (strncmp(name, text, len) == 0) return strdup(name);
  }
  return NULL;
}
static char *regs_generator(const char *text, int state) {
  static int list_index, len;
  char      *name;
  if (state == 0) {
    list_index = 0;
    len        = strlen(text);
  }
  while ((name = regs[list_index].regname)) {
    ++list_index;
    if (strncmp(name, text, len) == 0) return strdup(name);
  }
  return NULL;
}

static char **monitor_completion(const char *text, int start, int end) {
  if (start == 0)
    return rl_completion_matches(text, command_generator);
  else
    return rl_completion_matches(text, regs_generator);
}

void initialize_readline() {
  rl_readline_name                 = "remumonitor";
  rl_attempted_completion_function = monitor_completion;
}

static int find_command(char **command) {
  int length = 0;
  if (isfirstcomchar((*command)[length])) {
    ++length;
    while (iscomchar((*command)[length])) {
      ++length;
    }
  }
  if (length == 0) return -1;
  for (int i = 0; commands[i].abbr; ++i) {
    char *abbr = commands[i].abbr;
    char *name = commands[i].name;
    if (strncmp(*command, abbr, max(length, strlen(abbr))) == 0 ||
        strncmp(*command, name, max(length, strlen(name))) == 0) {
      *command += length;
      return i;
    }
  }
  return -2;
}

int monitor_execute(MACHINE *machine, char *command) {
  int list_index = find_command(&command);
  if (list_index == -1)
    return 1;
  else if (list_index >= 0) {
    return commands[list_index].func(machine, command);
  } else {
    printf("error: unrecognized command\n");
    return -1;
  }
}
