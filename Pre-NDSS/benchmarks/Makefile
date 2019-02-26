benchmark = \
		cmov

RV64_GCC = riscv64-unknown-elf-gcc
RV64_GCC_OPTIONS = -DPREALLOCATE=1 -mcmode=medany -static -std=gnu99 -O2 -ffast-math -fno-common -fno-builtin-printf
RV64_LINK = $(RV64_GCC) -T ...
RV64_LINK_OPTIONS = -static -nostdlib -nostartfiles -lm -lgcc -T ...
RV64_OBJDUMP = riscv64-unknown-elf-objdump --disassemble-all --disassemble-zeroes --section=.text --section=.text.startup --section=.data
