# RISC-V BOOM Project Template

This is a starter template for your own RISC-V BOOM project.

[BOOM](https://github.com/ucb-bar/riscv-boom) is a superscalar, out-of-order 
processor that implements RISC-V RV64G ISA. BOOM is a synthesizable core that 
targets ASIC processes, and is written in the 
[Chisel hardware construction language](http://chisel.eecs.berkeley.edu).

 Feature | BOOM
--- | ---
ISA | RISC-V (RV64G)
Synthesizable |√
FPGA |√
Parameterized |√
Floating Point (IEEE 754-2008) |√
Atomic Memory Op Support |√
Caches |√
Viritual Memory |√
Boots Linux |√
Privileged Arch v1.11 |√
External Debug |√

You can find more information about BOOM here:
([github.com/ucb-bar/riscv-boom](https://github.com/ucb-bar/riscv-boom)).

**Google group:** (https://groups.google.com/forum/#!forum/riscv-boom)


## Submodules and Subdirectories

The submodules and subdirectories for the project template are organized as
follows.

 * rocket-chip - contains code for the RocketChip generator, Chisel HCL, and FIRRTL
 * rocket-chip/riscv-tools - contains the code for the compiler toolchain and other infrastructure
 * boom - contains code for the BOOM core and tile.
 * scripts - bash scripts for initializing repo, building RISC-V toolchain
 * testchipip - contains the serial adapter, block device, and associated verilog and C++ code
 * verisim - directory in which Verilator simulations are compiled and run
 * vsim - directory in which Synopsys VCS simulations are compiled and run
 * src/main/scala - scala source files for your project extension can go here


## Getting Started

### Checking out the sources

After cloning this repo, you will need to initialize all of the submodules

    git clone https://github.com/ccelio/boom-template.git
    cd boom-template
    ./scripts/init-submodules.sh
    
### Building the tools

The tools repo contains the cross-compiler toolchain, frontend server, and
proxy kernel, which you will need in order to compile code to RISC-V
instructions and run them on your design. There are detailed instructions at
https://github.com/riscv/riscv-tools. But to get a basic installation that
will work with BOOM, just the following steps are necessary.

    # You may want to add the following two lines to your shell profile
    export RISCV=/path/to/install/dir
    export PATH=$RISCV/bin:$PATH

    cd boom-template
    ./scripts/build-tools.sh
    
    
### Compiling and running the Verilator simulation

To compile a BOOM simulator, run `make` in the "verisim" directory.
This will elaborate the BoomConfig from the boom.system project.

    cd verisim
    make

An executable called simulator-boom-system-BoomConfig will be produced.
You can then use this executable to run any compatible RV64G code. For instance,
to run one of the riscv-tools assembly tests.

    make output/rv64ui-p-simple.out
    
 Or execute the entire `riscv-tests` suite:
 
    make run
    
 Or just a smaller regression suite:
 
    make run-regression-tests
    
 If you would like to get a `.vpd` waveform, you can instead use:
 
    make output/rv64ui-p-simple.vpd
    
 Or:
 
    make run-debug

If you later create your own project, you can use environment variables to
build an alternate configuration. The different variables are

* PROJECT: The package that contains your test harness class
* CFG_PROJECT: The package that contains your config class
* GENERATOR_PROJECT: The package that contains your Generator class
* MODEL: The class name of your test harness
* CONFIG: The class name of your config

You can manually override them like this

    make PROJECT=yourproject CONFIG=YourConfig
    ./simulator-yourproject-YourConfig ...

### Running random tests with torture ###

RISC-V Torture is included as a submodule and includes the ability to test
BOOM. You can run a single test like so:

    make rgentest R_SIM=../vsim/simv-boom.system-BoomConfig

You can run a nightly test, which runs for a set amount of time or a set
number of failures like this:

    make rnight R_SIM=../vsim/simv-boom.system-BoomConfig OPTIONS="-C config/default.config -t 5 -m 30"
      
# FAQ #

### How do I use VCS instead of Verilator? ###

The `verisim` directory manages the Verilator build and run process. 

The `vsim` directory manages the VCS build and run process. 

In either directory you can build and then run the `riscv-tests` using `make && make debug`.

### How do I get a waveform? ###

The testharness/build-system is currently set up to provide a `vpd` waveform file.

In the `verisim` or `vsim` directories, instead of invoking `make`, you can invoke `make debug` 
to build a waveform-output-enabled BOOM simulator. The simulator will now be suffixed with `-debug`.
*Warning:* Verilator takes a very long time to compile with waveform output enabled.

To run *all* of the riscv-tests with waveform output, you can invoke `make run-debug`.

Individually, you run a specific test as `make output/rv64ui-p-simple.out` to run a regular test 
or `make output/rv64ui-p-simple.vpd` to generate a waveform.

Read the Makefile to find all of the special targets.

### BOOM takes forever to compile ###

You can add to your bash profile:

    export MAKEFLAGS="-j `echo \`nproc\`*2/2|bc`" 
    
This will spawn as many threads as you have cores when invoking `make`, speed up compilation, 
and run the `riscv-tests` in parallel. 

**Warning**: if you are performing VCS simulation you will burn through your precious licenses.

**Warning**: if you are writing a lot of data to the *.out files or *.vpd files you may hose your file system.

Unfortunately, many of the structures in an OOO processor scale worse than linearly. 
Also, some compilers struggle with large functions that arise when you turn the auto-generated
Verilog into straight-line C++ code that is flattened across the whole design. 
As such, VCS compiles much faster than Verilator (but runs much slower).

Here are some times as measured on my machine using the verilator simulator:

  * `make CONFIG=MegaBoomConfig run` takes 57 minutes.

  * `make CONFIG=BoomConfig run` takes 39 minutes.
  
  * `make CONFIG=SmallBoomConfig run` takes 15 minutes.

To improve the speed of your run-debug loop, you can instead invoke a smaller set of tests:

````
make run-regression-tests
````

### Help! BOOM isn't working! ###

First verify the software is not an issue. Run spike first:

````
# Verify it works on spike.
spike --isa=rv64imafd my_program

# Then we can run on BOOM.
./emulator-freechips.rocketchip.system-SmallBoomConfig my_program 
````

Also verify the riscv-tools you built is the one pointed to within 
the boom-template/rocket-chip/riscv-tools repository. Otherwise a 
version mismatch can easily occur!


### How do I debug BOOM? ###

I recommend opening up the waveform and starting with the following signals, 
located in `TestDriver.testHarness.dut.tile.core`:

   * debug_tsc_reg (cycle counter)
   * debug_irt_reg (retired instruction counter)
   * rob_io_commit_valids_* (the commit signals)
   * csr_io_pc (roughly the commit pc)
   * br_unit_brinfo_valid (was a branch/jalr resolved?)
   * br_unit_brinfo_mispredict (was a branch/jalr mispredicted?)
   * dis_valids_* (are any instructions being dispatched to the issue units?)
   * dis_uops_*_pc (what are the PCs of the dispatched instructions?)



# Additional Information from the Project-Template README

The original source of boom-template derives from (https://github.com/ucb-bar/project-template).
Please visit it to learn more about adding your own devices or submodules to this template.

