// See LICENSE.Berkeley for license details.

package freechips.rocketchip.util

import Chisel._
import chisel3.util.HasBlackBoxResource

/** This black-boxes a Clock Divider by 2.
  * The output clock is phase-aligned to the input clock.
  * If you use this in synthesis, make sure your sdc
  * declares that you want it to do the same.
  *
  * Because Chisel does not support
  * blocking assignments, it is impossible
  * to create a deterministic divided clock.
  */
class ClockDivider2 extends BlackBox with HasBlackBoxResource {
  val io = new Bundle {
    val clk_out = Clock(OUTPUT)
    val clk_in  = Clock(INPUT)
  }

  setResource("/vsrc/ClockDivider2.v")
}
class ClockDivider3 extends BlackBox with HasBlackBoxResource {
  val io = new Bundle {
    val clk_out = Clock(OUTPUT)
    val clk_in  = Clock(INPUT)
  }

  setResource("/vsrc/ClockDivider3.v")
}

/** Divide the clock by power of 2 times.
 *  @param pow2 divides the clock 2 ^ pow2 times
 *  WARNING: This is meant for simulation use only. */
class Pow2ClockDivider(pow2: Int) extends Module {
  val io = new Bundle {
    val clock_out = Clock(OUTPUT)
  }

  if (pow2 == 0) {
    io.clock_out := clock
  } else {
    val dividers = Seq.fill(pow2) { Module(new ClockDivider2) }

    dividers.init.zip(dividers.tail).map { case (last, next) =>
      next.io.clk_in := last.io.clk_out
    }

    dividers.head.io.clk_in := clock
    io.clock_out := dividers.last.io.clk_out
  }
}
