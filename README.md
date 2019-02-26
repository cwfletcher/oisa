# Data Oblivious ISA Extensions for Side Channel-Resistant and High Performance Computing (Hardware)

## 1. About this paper

This paper is published in Network and Distributed System Security Symposium (NDSS) in 2019. The eprint version can be found [here](https://eprint.iacr.org/2018/808.pdf).

## 2. Introduction

This is a modfied version of the RISCV-BOOM (Berkley Out-of-Order Machine). We augment both the ISA and the hardware to support the Data Oblivious ISA extension.  Recall from the paper, the ISA extension is broken into two parts:

1.) A mechanism for Software to tell Hardware what data is sensitive (Confidential).  This is implemented with a new Dynamic Information Flow Tracking scheme that requires tags to follow data in every pipeline stage (not just retirement).

2.) A mechanism for Hardware to tell Software what operations (instructions) may leak privacy.  This is implemented by augmenting existing instructions (and adding new instructions) with additional semantics to indicate whether each instruction operand is Safe/Unsafe to receive confidential data.

Since the first-priority goal of HW implementation is to evaluate frequency and hardware overhead, this version does not support certain instruction types listed in the paper.

## 3. Supported Instructions


| inst       | function                             | tag update                     | attribute                           | if implemented |
|------------|--------------------------------------|--------------------------------|-------------------------------------|----------------|
| cmov       | RD=MUX(RS1,RS2,RD)                   | RD.tag = RS1.tag \|\| RS2.tag  |                                     | yes            |
| seal       | RD=RS1                               | RD.tag = 1                     |                                     | yes            |
| unseal     | RD=RS1                               | RD.tag = 0                     | barrier                             | yes            |
| Int/FP ALU | RD=RS1 op RS2                        | RD.tag = RS1.tag \|\| RS2.tag  |                                     | yes            |
| BXX        | if (RS1 op RS2)      PC = PC + immed |                                | assert RS1.tag==0 assert RS2.tag==0 | yes            |
| JALR(JR)   | PC = RS1 + immed; RD = PC + 4        | RD.tag = 0                     | assert RS1.tag==0                   | yes            |
| JAL        | PC = PC + immed; RD = PC + 4         | RD.tag = 0                     |                                     | yes            |
| orld       | RD = mem[RS1 + immed]                | RD.tag = mem[RS1 + immed].tag  |                                     | no             |
| orst       | mem[RS1 + immed] = RS2               | mem[RS1 + immed].tag = RS2.tag |                                     | no             |

## 4. Usage
(How to use BOOM can be found [here](https://github.com/esperantotech/boom-template))
### 1) Clone the project, build a BOOM verilator and its corresponding RISC-V toolchain:
```
git clone https://github.com/jiyongyu/OISA_BOOM
cd OISA_BOOM
./scripts/init-submodules.sh
./scripts/build-tools.sh
cd verisim
make
```

### 2) Compile a program into RISCV binary using OISA primitives, run with SW simulator **spike**:
We provides several microbenchmarks, libraries and Makefiles in /benchmark.

To compile your program with a lite version of standard library(provided by original RISCV toolchain), use *Makefile*.
Using this lite standard library provides basic routines(printf, cstring functions, atol, memset, memcpy, etc.). It has the advantage of shrinking code size and simpify the debugging process.

You can compile your program, generate riscv binary, and run with spike simulator by:
```
make run
```
You can also disassemble your riscv binary by:
```
make dump
```
To compile your program with standard library, use *Makefile.pk* instead of *Makefile*

### 3) Compile a program into x86 binary using OISA primitives, run on local x86 machine:
To compile your program with standard library, use *Makefile.pk*.
You can compile your program, generate x86 binary and run on x86 machines by:
```
make x86_run
```
You can also compile your program, generate riscv binary, and run with spike simulator by:
```
make riscv_run
```

### 4) Debug your progarm using spike:
```
spike -d --isa=rv64g <your_riscv_binary>
```
More about debugging using spike can be found [here](https://github.com/riscv/riscv-isa-sim/)

### 5) Compile and run with Verilator simulation
```
cd verisim
make
```
For more information, please refer to original BOOM github repo [here](https://github.com/esperantotech/boom-template).


## 5. Future Works

We will update this project by adding support for more oblivious instruction types.
