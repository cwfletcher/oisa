#! /bin/bash

# top-level
git submodule update --init
# rocket-chip (skip tools)
(cd rocket-chip && git submodule update --init --recursive hardfloat chisel3 firrtl)
# riscv-tools (skip llvm)
(cd rocket-chip && git submodule update --init riscv-tools)
(cd rocket-chip/riscv-tools && git submodule update --init --recursive riscv-isa-sim riscv-fesvr riscv-pk riscv-opcodes riscv-tests riscv-gnu-toolchain riscv-openocd)
# torture submodules
(cd torture && git submodule update --init --recursive)
