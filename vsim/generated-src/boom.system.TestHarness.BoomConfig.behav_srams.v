
module tag_array_ext(
  input RW0_clk,
  input [5:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [7:0] RW0_wmask,
  input [175:0] RW0_wdata,
  output [175:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [5:0] reg_RW0_addr;
  reg [175:0] ram [63:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 64; initvar = initvar+1)
        ram[initvar] = {6 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      if (RW0_wmask[0]) ram[RW0_addr][21:0] <= RW0_wdata[21:0];
      if (RW0_wmask[1]) ram[RW0_addr][43:22] <= RW0_wdata[43:22];
      if (RW0_wmask[2]) ram[RW0_addr][65:44] <= RW0_wdata[65:44];
      if (RW0_wmask[3]) ram[RW0_addr][87:66] <= RW0_wdata[87:66];
      if (RW0_wmask[4]) ram[RW0_addr][109:88] <= RW0_wdata[109:88];
      if (RW0_wmask[5]) ram[RW0_addr][131:110] <= RW0_wdata[131:110];
      if (RW0_wmask[6]) ram[RW0_addr][153:132] <= RW0_wdata[153:132];
      if (RW0_wmask[7]) ram[RW0_addr][175:154] <= RW0_wdata[175:154];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [191:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random, $random, $random, $random, $random, $random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random, $random, $random, $random, $random, $random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[175:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module _T_39_ext(
  input W0_clk,
  input [7:0] W0_addr,
  input W0_en,
  input [127:0] W0_data,
  input [1:0] W0_mask,
  input R0_clk,
  input [7:0] R0_addr,
  input R0_en,
  output [127:0] R0_data
);

  reg reg_R0_ren;
  reg [7:0] reg_R0_addr;
  reg [127:0] ram [255:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 256; initvar = initvar+1)
        ram[initvar] = {4 {$random}};
      reg_R0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge R0_clk)
    reg_R0_ren <= R0_en;
  always @(posedge R0_clk)
    if (R0_en) reg_R0_addr <= R0_addr;
  always @(posedge W0_clk)
    if (W0_en) begin
      if (W0_mask[0]) ram[W0_addr][63:0] <= W0_data[63:0];
      if (W0_mask[1]) ram[W0_addr][127:64] <= W0_data[127:64];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [127:0] R0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      R0_random = {$random, $random, $random, $random};
      reg_R0_ren = R0_random[0];
    end
  `endif
  always @(posedge R0_clk) R0_random <= {$random, $random, $random, $random};
  assign R0_data = reg_R0_ren ? ram[reg_R0_addr] : R0_random[127:0];
  `else
  assign R0_data = ram[reg_R0_addr];
  `endif

endmodule

module tag_array_0_ext(
  input RW0_clk,
  input [6:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [3:0] RW0_wmask,
  input [79:0] RW0_wdata,
  output [79:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [6:0] reg_RW0_addr;
  reg [79:0] ram [127:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 128; initvar = initvar+1)
        ram[initvar] = {3 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      if (RW0_wmask[0]) ram[RW0_addr][19:0] <= RW0_wdata[19:0];
      if (RW0_wmask[1]) ram[RW0_addr][39:20] <= RW0_wdata[39:20];
      if (RW0_wmask[2]) ram[RW0_addr][59:40] <= RW0_wdata[59:40];
      if (RW0_wmask[3]) ram[RW0_addr][79:60] <= RW0_wdata[79:60];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [95:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random, $random, $random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random, $random, $random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[79:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module dataArraysB0_0_ext(
  input RW0_clk,
  input [8:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [63:0] RW0_wdata,
  output [63:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [8:0] reg_RW0_addr;
  reg [63:0] ram [511:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 512; initvar = initvar+1)
        ram[initvar] = {2 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      ram[RW0_addr][63:0] <= RW0_wdata[63:0];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [63:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random, $random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random, $random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[63:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module bimDataArray__ext(
  input RW0_clk,
  input [8:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [7:0] RW0_wmask,
  input [7:0] RW0_wdata,
  output [7:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [8:0] reg_RW0_addr;
  reg [7:0] ram [511:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 512; initvar = initvar+1)
        ram[initvar] = {1 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      if (RW0_wmask[0]) ram[RW0_addr][0:0] <= RW0_wdata[0:0];
      if (RW0_wmask[1]) ram[RW0_addr][1:1] <= RW0_wdata[1:1];
      if (RW0_wmask[2]) ram[RW0_addr][2:2] <= RW0_wdata[2:2];
      if (RW0_wmask[3]) ram[RW0_addr][3:3] <= RW0_wdata[3:3];
      if (RW0_wmask[4]) ram[RW0_addr][4:4] <= RW0_wdata[4:4];
      if (RW0_wmask[5]) ram[RW0_addr][5:5] <= RW0_wdata[5:5];
      if (RW0_wmask[6]) ram[RW0_addr][6:6] <= RW0_wdata[6:6];
      if (RW0_wmask[7]) ram[RW0_addr][7:7] <= RW0_wdata[7:7];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [31:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[7:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module btb_tag_array_ext(
  input RW0_clk,
  input [7:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [19:0] RW0_wdata,
  output [19:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [7:0] reg_RW0_addr;
  reg [19:0] ram [255:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 256; initvar = initvar+1)
        ram[initvar] = {1 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      ram[RW0_addr][19:0] <= RW0_wdata[19:0];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [31:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[19:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module btb_data_array_ext(
  input RW0_clk,
  input [7:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [44:0] RW0_wdata,
  output [44:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [7:0] reg_RW0_addr;
  reg [44:0] ram [255:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 256; initvar = initvar+1)
        ram[initvar] = {2 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      ram[RW0_addr][44:0] <= RW0_wdata[44:0];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [63:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random, $random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random, $random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[44:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module TageTableDataArray_ext(
  input W0_clk,
  input [9:0] W0_addr,
  input W0_en,
  input [14:0] W0_data,
  input R0_clk,
  input [9:0] R0_addr,
  input R0_en,
  output [14:0] R0_data
);

  reg reg_R0_ren;
  reg [9:0] reg_R0_addr;
  reg [14:0] ram [1023:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 1024; initvar = initvar+1)
        ram[initvar] = {1 {$random}};
      reg_R0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge R0_clk)
    reg_R0_ren <= R0_en;
  always @(posedge R0_clk)
    if (R0_en) reg_R0_addr <= R0_addr;
  always @(posedge W0_clk)
    if (W0_en) begin
      ram[W0_addr][14:0] <= W0_data[14:0];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [31:0] R0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      R0_random = {$random};
      reg_R0_ren = R0_random[0];
    end
  `endif
  always @(posedge R0_clk) R0_random <= {$random};
  assign R0_data = reg_R0_ren ? ram[reg_R0_addr] : R0_random[14:0];
  `else
  assign R0_data = ram[reg_R0_addr];
  `endif

endmodule

module l2_tlb_ram_ext(
  input RW0_clk,
  input [8:0] RW0_addr,
  input RW0_en,
  input RW0_wmode,
  input [44:0] RW0_wdata,
  output [44:0] RW0_rdata
);

  reg reg_RW0_ren;
  reg [8:0] reg_RW0_addr;
  reg [44:0] ram [511:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 512; initvar = initvar+1)
        ram[initvar] = {2 {$random}};
      reg_RW0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge RW0_clk)
    reg_RW0_ren <= RW0_en && !RW0_wmode;
  always @(posedge RW0_clk)
    if (RW0_en && !RW0_wmode) reg_RW0_addr <= RW0_addr;
  always @(posedge RW0_clk)
    if (RW0_en && RW0_wmode) begin
      ram[RW0_addr][44:0] <= RW0_wdata[44:0];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [63:0] RW0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      RW0_random = {$random, $random};
      reg_RW0_ren = RW0_random[0];
    end
  `endif
  always @(posedge RW0_clk) RW0_random <= {$random, $random};
  assign RW0_rdata = reg_RW0_ren ? ram[reg_RW0_addr] : RW0_random[44:0];
  `else
  assign RW0_rdata = ram[reg_RW0_addr];
  `endif

endmodule

module mem_ext(
  input W0_clk,
  input [24:0] W0_addr,
  input W0_en,
  input [63:0] W0_data,
  input [7:0] W0_mask,
  input R0_clk,
  input [24:0] R0_addr,
  input R0_en,
  output [63:0] R0_data
);

  reg reg_R0_ren;
  reg [24:0] reg_R0_addr;
  reg [63:0] ram [33554431:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 33554432; initvar = initvar+1)
        ram[initvar] = {2 {$random}};
      reg_R0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge R0_clk)
    reg_R0_ren <= R0_en;
  always @(posedge R0_clk)
    if (R0_en) reg_R0_addr <= R0_addr;
  always @(posedge W0_clk)
    if (W0_en) begin
      if (W0_mask[0]) ram[W0_addr][7:0] <= W0_data[7:0];
      if (W0_mask[1]) ram[W0_addr][15:8] <= W0_data[15:8];
      if (W0_mask[2]) ram[W0_addr][23:16] <= W0_data[23:16];
      if (W0_mask[3]) ram[W0_addr][31:24] <= W0_data[31:24];
      if (W0_mask[4]) ram[W0_addr][39:32] <= W0_data[39:32];
      if (W0_mask[5]) ram[W0_addr][47:40] <= W0_data[47:40];
      if (W0_mask[6]) ram[W0_addr][55:48] <= W0_data[55:48];
      if (W0_mask[7]) ram[W0_addr][63:56] <= W0_data[63:56];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [63:0] R0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      R0_random = {$random, $random};
      reg_R0_ren = R0_random[0];
    end
  `endif
  always @(posedge R0_clk) R0_random <= {$random, $random};
  assign R0_data = reg_R0_ren ? ram[reg_R0_addr] : R0_random[63:0];
  `else
  assign R0_data = ram[reg_R0_addr];
  `endif

endmodule

module mem_0_ext(
  input W0_clk,
  input [8:0] W0_addr,
  input W0_en,
  input [63:0] W0_data,
  input [7:0] W0_mask,
  input R0_clk,
  input [8:0] R0_addr,
  input R0_en,
  output [63:0] R0_data
);

  reg reg_R0_ren;
  reg [8:0] reg_R0_addr;
  reg [63:0] ram [511:0];
  `ifdef RANDOMIZE_MEM_INIT
    integer initvar;
    initial begin
      #0.002 begin end
      for (initvar = 0; initvar < 512; initvar = initvar+1)
        ram[initvar] = {2 {$random}};
      reg_R0_addr = {1 {$random}};
    end
  `endif
  integer i;
  always @(posedge R0_clk)
    reg_R0_ren <= R0_en;
  always @(posedge R0_clk)
    if (R0_en) reg_R0_addr <= R0_addr;
  always @(posedge W0_clk)
    if (W0_en) begin
      if (W0_mask[0]) ram[W0_addr][7:0] <= W0_data[7:0];
      if (W0_mask[1]) ram[W0_addr][15:8] <= W0_data[15:8];
      if (W0_mask[2]) ram[W0_addr][23:16] <= W0_data[23:16];
      if (W0_mask[3]) ram[W0_addr][31:24] <= W0_data[31:24];
      if (W0_mask[4]) ram[W0_addr][39:32] <= W0_data[39:32];
      if (W0_mask[5]) ram[W0_addr][47:40] <= W0_data[47:40];
      if (W0_mask[6]) ram[W0_addr][55:48] <= W0_data[55:48];
      if (W0_mask[7]) ram[W0_addr][63:56] <= W0_data[63:56];
    end
  `ifdef RANDOMIZE_GARBAGE_ASSIGN
  reg [63:0] R0_random;
  `ifdef RANDOMIZE_MEM_INIT
    initial begin
      #0.002 begin end
      R0_random = {$random, $random};
      reg_R0_ren = R0_random[0];
    end
  `endif
  always @(posedge R0_clk) R0_random <= {$random, $random};
  assign R0_data = reg_R0_ren ? ram[reg_R0_addr] : R0_random[63:0];
  `else
  assign R0_data = ram[reg_R0_addr];
  `endif

endmodule
