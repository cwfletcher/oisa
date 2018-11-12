//******************************************************************************
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
package boom.common

import chisel3._
import freechips.rocketchip.config.Parameters

abstract class BoomModule(implicit p: Parameters) extends freechips.rocketchip.tile.CoreModule()(p)
  with HasBoomCoreParameters

class BoomBundle(implicit val p: Parameters) extends freechips.rocketchip.util.ParameterizedBundle()(p)
  with HasBoomCoreParameters

