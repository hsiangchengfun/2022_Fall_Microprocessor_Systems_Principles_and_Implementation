# ======================================================================
#  Builds the boot code memory file for Xilinx Vivado EDA tools.
#
#  Chun-Jen Tsai, Nov/04/2019
# ======================================================================

CROSS = riscv32-unknown-elf
CCPATH = $(RISCV)/bin

CC = $(CCPATH)/$(CROSS)-gcc
LD = $(CCPATH)/$(CROSS)-ld
AR = $(CCPATH)/$(CROSS)-ar
OC = $(CCPATH)/$(CROSS)-objcopy
OD = $(CCPATH)/$(CROSS)-objdump
SP = $(CCPATH)/$(CROSS)-strip

GCCVERSION = $(shell $(CC) --version | grep gcc | sed 's/^.* //g')

LIBC = ../elibc
LIBC_OBJS = $(LIBC)/crt0.o $(LIBC)/stdio.o $(LIBC)/stdlib.o $(LIBC)/string.o $(LIBC)/time.o $(LIBC)/uart.o

CCFLAGS = -Wall -O2 -I$(LIBC) -fomit-frame-pointer -fno-builtin -march=rv32ima -mstrict-align -mabi=ilp32
LDFLAGS = -L. -T$*.ld -Map=$*.map
LD_SOFT_FP = -L$(RISCV)/lib/gcc/riscv32-unknown-elf/$(GCCVERSION) -lgcc

OCFLAGS = -O binary
ODFLAGS = -d

PROJ = dhry
OBJS = dhry_1.o dhry_2.o

all:
	make $(PROJ).mem

clean:
	rm -f *.mem *.objdump *.map *.o *.a

%.o: %.c $(PROJ).ld
	$(CC) $(CCFLAGS) -c $< -o $@

libelibc.a : $(LIBC_OBJS)
	$(AR) r libelibc.a $(LIBC_OBJS)
	rm $^

%.elf: $(OBJS) libelibc.a
	$(LD) $(LDFLAGS) $(OBJS) -lelibc $(LD_SOFT_FP) -o $@
	$(SP) $(PROJ).elf
	rm $^

%.bin: $(PROJ).elf
	$(OC) $(OCFLAGS) $< $@
	$(OD) $(ODFLAGS) $< > $(PROJ).objdump
	rm $<

%.mem: $(PROJ).bin
	$(eval BIN_SIZE = $(shell stat -c %s $(PROJ).bin))
	hexdump -ve '1/4 "%08x\n"' -n $(BIN_SIZE) $(PROJ).bin > $@
	rm $<

