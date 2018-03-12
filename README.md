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

To compile a BOOM simulator, run make in the "verisim" directory.
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
      
    

# Additional Information from the Project-Template README

The original source of boom-template derives from (https://github.com/ucb-bar/project-template).
Please visit it to learn more about adding your own devices or submodules to this template.

