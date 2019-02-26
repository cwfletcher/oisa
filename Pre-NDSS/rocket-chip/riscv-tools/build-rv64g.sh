#! /bin/bash
#
# Script to build RISC-V ISA simulator, proxy kernel, and GNU toolchain.
# Tools will be installed to $RISCV.

. build.common

echo Starting RISC-V Toolchain build process
echo Tools will be installed to $RISCV.

build_project riscv-fesvr --prefix=$RISCV
build_project riscv-isa-sim --prefix=$RISCV --with-fesvr=$RISCV --with-isa=rv64imafd
build_project riscv-gnu-toolchain --prefix=$RISCV --with-arch=rv64imafd
CC= CXX= build_project riscv-pk --prefix=$RISCV --host=riscv64-unknown-elf
build_project riscv-openocd --prefix=$RISCV --enable-remote-bitbang --enable-jtag_vpi --disable-werror
build_project riscv-tests --prefix=$RISCV/riscv64-unknown-elf

echo "RISC-V Toolchain installation completed!"
