BINDIR=.
INCLUDEDIR=./header
SRCDIR=./src
BUILDDIR=./build

TARGET=remu
CFLAGS=-O0 -g -Wall -lreadline
CPPFLAGS=
ASFLAGS=
LDFLAGS=

INCFLAGS=-I$(INCLUDEDIR)

CFLAGS+=$(INCFLAGS)

CC=gcc
AS=as
LD=ld
GDB=gdb
OBJDUMP=objdump
OBJCOPY=objcopy

BINTARGET=$(addprefix $(BINDIR)/,$(TARGET))
SRCS_C=$(wildcard $(SRCDIR)/*.c)
SRCS_ASM=$(wildcard $(SRCDIR)/*.S)
OBJS=$(patsubst %.c,$(BUILDDIR)/%.o,$(notdir $(SRCS_C)))
OBJS+=$(patsubst %.S,$(BUILDDIR)/%.o,$(notdir $(SRCS_ASM)))
DEPS=$(patsubst %.c,$(BUILDDIR)/%.d,$(notdir $(SRCS_C)))
DEPS+=$(patsubst %.S,$(BUILDDIR)/%.d,$(notdir $(SRCS_ASM)))

.PHONY: all
all: $(BINTARGET)
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
	@./$(BINTARGET)

.PHONY: debug
debug: all
	@$(GDB) $(BINTARGET) -q

.PHONY: clean
clean:
	@rm -f $(BINTARGET) $(OBJS) $(DEPS)
	@echo 'Done'

RV64IM.o: RV64IM.S
	riscv64-elf-as -o $@ $< -march=rv64im -mabi=lp64
RV64IM.elf: RV64IM.o
	riscv64-elf-ld -o $@ $< -Ttext=0x80000000
hello.bin: RV64IM.elf
	riscv64-elf-objcopy $< -O binary $@ -S

-include $(DEPS)
