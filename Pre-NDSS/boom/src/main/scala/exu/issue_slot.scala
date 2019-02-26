//******************************************************************************
// Copyright (c) 2015, The Regents of the University of California (Regents).
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
// RISCV Processor Issue Slot Logic
//--------------------------------------------------------------------------
//------------------------------------------------------------------------------
//
// Note: stores (and AMOs) are "broken down" into 2 uops, but stored within a single issue-slot.
// TODO XXX make a separate issueSlot for MemoryIssueSlots, and only they break apart stores.
// TODO Disable ldspec for FP queue.


package boom.exu

import chisel3._
import chisel3.util.Valid
import FUConstants._
import freechips.rocketchip.config.Parameters
import boom.common._
import boom.util._

class IssueSlotIO(num_wakeup_ports: Int)(implicit p: Parameters) extends BoomBundle()(p)
{
   val valid          = Output(Bool())
   val will_be_valid  = Output(Bool()) // TODO code review, do we need this signal so explicitely?
   val request        = Output(Bool())
   val request_hp     = Output(Bool())
   val grant          = Input(Bool()) // note that issue logic grants this slot only after this slot itself is ready
                                      // aka if granted, then this slot is supposed to issue at this cycle(except poisoned)

   val brinfo         = Input(new BrResolutionInfo())
   val kill           = Input(Bool()) // pipeline flush
   val clear          = Input(Bool()) // entry being moved elsewhere (not mutually exclusive with grant)
   val ldspec_miss    = Input(Bool()) // Previous cycle's speculative load wakeup was mispredicted.

   val wakeup_dsts    = Flipped(Vec(num_wakeup_ports, Valid(new IqWakeup(PREG_SZ))))
   val ldspec_dst     = Flipped(Valid(UInt(width=PREG_SZ.W)))
   val in_uop         = Flipped(Valid(new MicroOp())) // if valid, this WILL overwrite an entry!
   val updated_uop    = Output(new MicroOp()) // the updated slot uop; will be shifted upwards in a collasping queue.
   val uop            = Output(new MicroOp()) // the current Slot's uop. Sent down the pipeline when issued.

   val debug = {
     val result = new Bundle {
       val p1 = Bool()
       val p2 = Bool()
       val p3 = Bool()
       val pd = Bool()
       val state = UInt(width=2.W)
    }
    Output(result)
  }

   override def cloneType = new IssueSlotIO(num_wakeup_ports)(p).asInstanceOf[this.type]
}

class IssueSlot(num_slow_wakeup_ports: Int)(implicit p: Parameters)
   extends BoomModule()(p)
   with IssueUnitConstants
{
   val io = IO(new IssueSlotIO(num_slow_wakeup_ports))

   // slot invalid?
   // slot is valid, holding 1 uop
   // slot is valid, holds 2 uops (like a store)
   def isInvalid = slot_state === s_invalid
   def isValid = slot_state =/= s_invalid

   val updated_state      = Wire(UInt()) // the next state of this slot (which might then get moved to a new slot)
   val updated_uopc       = Wire(UInt()) // the next uopc of this slot (which might then get moved to a new slot)
   val updated_lrs1_rtype = Wire(UInt()) // the next reg type of this slot (which might then get moved to a new slot)
   val updated_lrs2_rtype = Wire(UInt()) // the next reg type of this slot (which might then get moved to a new slot)
   val updated_dst_rtype  = Wire(UInt()) // the next reg type of this slot (which might then get moved to a new slot)

   val slot_state         = RegInit(s_invalid)
   val slot_p1            = RegInit(false.B)  // is p1(RS1) ready?
   val slot_p2            = RegInit(false.B)  // is p2(RS2) ready?
   val slot_p3            = RegInit(false.B)  // is p3(RS3) ready? (only for FMA etc.)
   val slot_pd            = RegInit(false.B)  // is stale pdst(RD) ready? (only for cmov)

   // Poison if woken up by speculative load.
   // Poison lasts 1 cycle (as ldMiss will come on the next cycle).
   // SO if poisoned is true, set it to false!
   val slot_p1_poisoned   = RegInit(false.B)
   val slot_p2_poisoned   = RegInit(false.B)
   val slot_pd_poisoned   = RegInit(false.B)

   val slotUop = Reg(init = NullMicroOp)

   //-----------------------------------------------------------------------------
   // next slot state computation
   // compute the next state for THIS entry slot (in a collasping queue, the
   // current uop may get moved elsewhere, and a new slot can get moved into
   // here

   when (io.kill)
   {
      slot_state := s_invalid
   }
   .elsewhen (io.in_uop.valid)
   {
      slot_state := io.in_uop.bits.iw_state
   }
   .elsewhen (io.clear)
   {
      slot_state := s_invalid
   }
   .otherwise
   {
      slot_state := updated_state
   }

   //-----------------------------------------------------------------------------
   // "update" state computation
   // compute the next state for the micro-op in this slot. This micro-op may
   // be moved elsewhere, so the "updated_state" travels with it.

   // defaults
   updated_state := slot_state
   updated_uopc := slotUop.uopc
   updated_lrs1_rtype := slotUop.lrs1_rtype
   updated_lrs2_rtype := slotUop.lrs2_rtype
   updated_dst_rtype  := slotUop.dst_rtype

   when (io.kill)
   {
      updated_state := s_invalid
   }
   .elsewhen (
      (io.grant && (slot_state === s_valid_1)) ||                       // granted and slot holds a valid uop(s_valid_1)
      (io.grant && (slot_state === s_valid_2) && slot_p1 && slot_p2))   // granted and slot holds a store like uop; two ops are ready 
   {
      // try to issue this uop.
      when (!(io.ldspec_miss && (slot_p1_poisoned || slot_p2_poisoned)))// ldspec is right, p1 & p2 is not poisoned 
      {
        updated_state := s_invalid                                      // this slot will be issued next cycle(state->invalid)
      }
   }
   .elsewhen (io.grant && (slot_state === s_valid_2))                   // granted and slot holds a store like uops that is broken into two micro-ops
   {
      when (!(io.ldspec_miss && (slot_p1_poisoned || slot_p2_poisoned)))// ldspec is right, p1 & p2 is not poisoned
      {
         updated_state := s_valid_1
         when (slot_p1)
         {
            slotUop.uopc := uopSTD
            updated_uopc := uopSTD
            slotUop.lrs1_rtype := RT_X
            updated_lrs1_rtype := RT_X
         }
         .otherwise
         {
            slotUop.lrs2_rtype := RT_X
            updated_lrs2_rtype := RT_X
         }
      }
   }

   when (io.in_uop.valid) // there is a uop moving from lower-priority slot to this slot(so this slot must be invalid at this cycle)
   {
      slotUop := io.in_uop.bits   // uop in this slot will be the incoming uop
      assert (isInvalid || io.clear || io.kill, "trying to overwrite a valid issue slot.")
   }

   // Wakeup Compare Logic
   // Namely other registers indicating whether registers for operands are ready & poisoned

   // these signals are the "next_p*" for the current slot's micro-op.
   // they are important for shifting the current slotUop up to an other entry.
   val updated_p1 = WireInit(slot_p1) // next p1 ready signal, used to update slot_p1
   val updated_p2 = WireInit(slot_p2) // next p2 ready signal, used to update slot_p2
   val updated_p3 = WireInit(slot_p3) // next p3 ready signal, used to update slot_p3
   val updated_pd = WireInit(slot_pd) // next pd ready signal, used to update slot_pd(only useful for cmov)
   // Even if slot is poisoned, set it to false immediately.
   val updated_p1_poisoned = WireInit(false.B)
   val updated_p2_poisoned = WireInit(false.B)
   val updated_pd_poisoned = WireInit(false.B)

   when (io.in_uop.valid)
   {
      slot_p1 := !(io.in_uop.bits.prs1_busy)  // p1 ready will be !(incoming uop's prs1_busy)
      slot_p2 := !(io.in_uop.bits.prs2_busy)  // p2 ready will be !(incoming uop's prs2_busy)
      slot_p3 := !(io.in_uop.bits.prs3_busy)  // p3 ready will be !(incoming uop's prs3_busy)
      slot_p1_poisoned := io.in_uop.bits.iw_p1_poisoned // next cycle p1_poison will be incoming uop's p1_poison
      slot_p2_poisoned := io.in_uop.bits.iw_p2_poisoned // next cycle p2_poison will be incoming uop's p2_poison
      when (io.in_uop.bits.uopc === uopCMOV || io.in_uop.bits.uopc === uopCMOVW) {
         slot_pd := !(io.in_uop.bits.pdst_busy)  // pd ready will be !(incoming uop's pdst_busy)
         slot_pd_poisoned := io.in_uop.bits.iw_pd_poisoned // next cycle pd_poison will be incoming uop's pd_poison
      }
      .otherwise {
         slot_pd := true.B
         slot_pd_poisoned := false.B
      }
   }
   .otherwise
   {
      slot_p1 := updated_p1   // update p1 ready with new p1 ready signal
      slot_p2 := updated_p2   // update p2 ready with new p2 ready signal
      slot_p3 := updated_p3   // update p3 ready with new p3 ready signal
      slot_p1_poisoned := updated_p1_poisoned
      slot_p2_poisoned := updated_p2_poisoned
      when (slotUop.uopc === uopCMOV || slotUop.uopc === uopCMOVW) {
         slot_pd := updated_pd
         slot_pd_poisoned := updated_pd_poisoned
      }
   }

   for (i <- 0 until num_slow_wakeup_ports)
   {
      when (io.wakeup_dsts(i).valid &&                        // if the wakeup signal is valid
         (io.wakeup_dsts(i).bits.pdst === slotUop.pop1) &&    // and wakeup dest register match register of op1
         !(io.ldspec_miss && io.wakeup_dsts(i).bits.poisoned))// and it's not a mis-speculation
      {
         updated_p1 := true.B // p1 ready will be true next cycle
      }
      when (io.wakeup_dsts(i).valid &&                        // if the wakeup signal is valid
         (io.wakeup_dsts(i).bits.pdst === slotUop.pop2) &&    // and wakeup dest register match register of op2
         !(io.ldspec_miss && io.wakeup_dsts(i).bits.poisoned))// and it's not a mis-speculation
      {
         updated_p2 := true.B // p2 ready will be true next cycle
      }
      when (io.wakeup_dsts(i).valid &&
         (io.wakeup_dsts(i).bits.pdst === slotUop.pdst) &&
         !(io.ldspec_miss && io.wakeup_dsts(i).bits.poisoned))
      {
         updated_pd := true.B
      }
      when (io.wakeup_dsts(i).valid &&                        // if the wakeup signal is valid
         (io.wakeup_dsts(i).bits.pdst === slotUop.pop3))      // and wakeup dest register match register of op3
      {
         updated_p3 := true.B // p3 ready will be true next cycle(only useful for FMA)
      }
   }

   // TODO disable if FP IQ.
   when (io.ldspec_dst.valid && io.ldspec_dst.bits === slotUop.pop1 && slotUop.lrs1_rtype === RT_FIX)
   {
      updated_p1 := true.B          // p1 will be ready
      updated_p1_poisoned := true.B // but p1 is poisoned by speculatively executed load
      assert (!slot_p1_poisoned)
   }
   when (io.ldspec_dst.valid && io.ldspec_dst.bits === slotUop.pop2 && slotUop.lrs2_rtype === RT_FIX)
   {
      updated_p2 := true.B          // p2 will be ready
      updated_p2_poisoned := true.B // but p2 is poisoned by speculatively executed load
      assert (!slot_p2_poisoned)
   }
   when (io.ldspec_dst.valid && io.ldspec_dst.bits === slotUop.pdst && slotUop.dst_rtype === RT_FIX)
   {
      updated_pd := true.B
      updated_pd_poisoned := true.B
      assert (!slot_pd_poisoned)
   }

   when (io.ldspec_miss && slot_p1_poisoned)
   {
      updated_p1 := false.B         // the speculatively executed load turns out to be mis-speculation. 
                                    // p1 will not be ready
   }
   when (io.ldspec_miss && slot_p2_poisoned)
   {
      updated_p2 := false.B         // the speculatively executed load turns out to be mis-sepculation.
                                    // p2 will not be ready
   }
   when (io.ldspec_miss && slot_pd_poisoned)
   {
      updated_pd := false.B
   }


   // Handle branch misspeculations
   val updated_br_mask = GetNewBrMask(io.brinfo, slotUop)

   // was this micro-op killed by a branch? if yes, we can't let it be valid if
   // we compact it into an other entry
   when (IsKilledByBranch(io.brinfo, slotUop))
   {
      updated_state := s_invalid
   }

   when (!io.in_uop.valid)
   {
      slotUop.br_mask := updated_br_mask
   }


   //-------------------------------------------------------------
   // Request Logic(whether all operands are ready)
   // this slot is ready to issue so it sends request to issue logic. however it cannot issue until the issue logic grant it
   io.request := isValid && slot_p1 && slot_p2 && slot_p3 && slot_pd && !io.kill

   val high_priority = slotUop.is_br_or_jmp
   io.request_hp := false.B

   when (slot_state === s_valid_1)
   {
      io.request := slot_p1 && slot_p2 && slot_p3 && slot_pd && !io.kill // all operands are ready; no kill signal at current cycle
   }
   .elsewhen (slot_state === s_valid_2)
   {
      io.request := (slot_p1 || slot_p2)  && !io.kill // one operand is ready; not kill signal at current cycle
   }
   .otherwise // slot_state === s_invalid
   {
      io.request := false.B
   }


   //assign outputs
   io.valid := isValid
   io.uop := slotUop
   io.uop.iw_p1_poisoned := slot_p1_poisoned
   io.uop.iw_p2_poisoned := slot_p2_poisoned
   io.uop.iw_pd_poisoned := slot_pd_poisoned


   // micro-op will vacate due to grant.
   val may_vacate = io.grant && ((slot_state === s_valid_1) || (slot_state === s_valid_2) && slot_p1 && slot_p2 && slot_pd)
   val squash_grant = io.ldspec_miss && (slot_p1_poisoned || slot_p2_poisoned || slot_pd_poisoned)
   io.will_be_valid := isValid && !(may_vacate && !squash_grant)


   // the updated slot uop; will be shifted upwards in a collasping queue.
   io.updated_uop           := slotUop
   io.updated_uop.iw_state  := updated_state
   io.updated_uop.uopc      := updated_uopc
   io.updated_uop.lrs1_rtype:= updated_lrs1_rtype
   io.updated_uop.lrs2_rtype:= updated_lrs2_rtype
   io.updated_uop.br_mask   := updated_br_mask
   io.updated_uop.prs1_busy := !updated_p1
   io.updated_uop.prs2_busy := !updated_p2
   io.updated_uop.prs3_busy := !updated_p3
   io.updated_uop.pdst_busy := !updated_pd
   io.updated_uop.iw_p1_poisoned := updated_p1_poisoned
   io.updated_uop.iw_p2_poisoned := updated_p2_poisoned
   io.updated_uop.iw_pd_poisoned := updated_pd_poisoned

   // the current Slot's uop. Sent down the pipeline when issued.
   when (slot_state === s_valid_2)
   {
      when (slot_p1 && slot_p2)
      {
         ; // send out the entire instruction as one uop
      }
      .elsewhen (slot_p1)
      {
         io.uop.uopc := slotUop.uopc
         io.uop.lrs2_rtype := RT_X
      }
      .elsewhen (slot_p2)
      {
         io.uop.uopc := uopSTD
         io.uop.lrs1_rtype := RT_X
      }
   }

   // debug outputs
   io.debug.p1 := slot_p1
   io.debug.p2 := slot_p2
   io.debug.p3 := slot_p3
   io.debug.pd := slot_pd
   io.debug.state := slot_state

   override val compileOptions = chisel3.core.ExplicitCompileOptions.NotStrict.copy(explicitInvalidate = true)
}

