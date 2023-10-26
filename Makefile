BINDIR=.
INCLUDEDIR=./header
SRCDIR=./src
BUILDDIR=./build

TARGET=remu
CFLAGS=-O0 -g -Wall -lreadline
CPPFLAGS=

INCFLAGS=-I$(INCLUDEDIR)

CFLAGS+=$(INCFLAGS)

CC=gcc
GDB=gdb

BINTARGET=$(addprefix $(BINDIR)/,$(TARGET))
SRCS_C=$(wildcard $(SRCDIR)/*.c)
SRCS_ASM=$(wildcard $(SRCDIR)/*.S)
OBJS=$(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(SRCS_C)))
OBJS+=$(patsubst %.S,$(BUILDDIR)/%.o,$(notdir $(SRCS_ASM)))
DEPS=$(patsubst %.c,$(BUILDDIR)/%.d,$(notdir $(SRCS_C)))
DEPS+=$(patsubst %.S,$(BUILDDIR)/%.d,$(notdir $(SRCS_ASM)))

RISCVDIR=./test
RISCV_PREFIX=riscv64-linux-gnu-

RAS=$(RISCV_PREFIX)as
RLD=$(RISCV_PREFIX)ld
ROBJCOPY=$(RISCV_PREFIX)objcopy
ROBJDUMP=$(RISCV_PREFIX)objdump

RSRCS=$(wildcard $(RISCVDIR)/*.S)
ROBJS=$(patsubst %.S,%.o,$(RSRCS))
RELF=$(RISCVDIR)/RV64IM.elf
RBIN=$(RISCVDIR)/RV64IM.bin

.PHONY: all
all: $(BINTARGET) $(RBIN)
	@echo 'Done'

$(BINTARGET): $(OBJS)
	@$(CC) -o $@ $^ $(CPPFLAGS) $(CFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@$(CC) -o $@ -c $< $(CPPFLAGS) $(CFLAGS)
$(BUILDDIR)/%.o: $(SRCDIR)/%.S
	@$(CC) -o $@ -c $< $(CPPFLAGS) $(CFLAGS)

$(BUILDDIR)/%.d: $(SRCDIR)/%.c
	@set -e; rm -f $@; \
	 $(CC) -MM -MF $@.$$$$ $< $(CFLAGS); \
	 sed 's,\($*\.o\)[ :]*,$(BUILDDIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$
$(BUILDDIR)/%.d: $(SRCDIR)/%.S
	@set -e; rm -f $@; \
	 $(CC) -MM -MF $@.$$$$ $< $(CFLAGS); \
	 sed 's,\($*\.o\)[ :]*,$(BUILDDIR)/\1 $@ : ,g' < $@.$$$$ > $@; \
	 rm -f $@.$$$$

.PHONY: run
run: all
	@./$(BINTARGET) -f $(RBIN) -m no-monitor

.PHONY: debug
debug: all
	@$(GDB) -q --args $(BINTARGET) -f $(RBIN) -m no-monitor

.PHONY: clean
clean:
	@rm -f $(BINTARGET) $(OBJS) $(DEPS) $(ROBJS) $(RBIN) $(RELF)
	@echo 'Done'

$(RBIN): $(RELF)
	@$(ROBJCOPY) $< -O binary $@ -S
$(RELF): $(ROBJS)
	@$(RLD) -o $@ $< -Ttext=0x80000000
$(RISCVDIR)/%.o: $(RISCVDIR)/%.S
	@$(RAS) -o $@ $< -march=rv64im -mabi=lp64
.PHONY: code
code:	$(RELF)
	@$(ROBJDUMP) -S $(RELF) | less

-include $(DEPS)
