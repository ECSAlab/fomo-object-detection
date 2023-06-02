/*
module quant 
(
  input  [31:0] acc,
  input  [31:0] qmul,  
  input  [3:0]  shift,
  input  [31:0] offset,
  output  [31:0] out
);
localparam min = $signed(-128);
localparam max = $signed(127);

reg signed [31:0] mask;
reg signed [31:0] remainder;
reg signed [31:0] threshold;
reg signed [31:0] dividend;
reg signed [63:0] dot;


always @(*) begin
    //if(en) begin
    //round top
    dot = $signed(acc) * $signed(qmul);
    dividend = ($signed({dot[63:32], dot[31:0]}) + 32'sh40000000) >> 31;
    //round top
    mask = ~({32 {1'b1}} << shift);
    remainder = dividend & mask;
    threshold = (mask >> 1) + dividend[31];
    out = $signed($signed(dividend) >>> shift);
    //result = $signed($signed(dividend) >>> shift);
    if (remainder > threshold)
        out = out + 1'b1;
        //result = result + 1'b1;

    out = out + $signed(offset);
    //result = result + $signed(offset);
    
    if ($signed(out) < min)
    //if ($signed(result) < min)
        out = min;
    else if ($signed(out) > max)
    //else if ($signed(result) > max)
        out = max;
    //end
end


endmodule


module quant 
(
  input  [31:0] acc,
  input  [31:0] qmul,  
  input  [3:0]  shift,
  input  [31:0] offset,
  output [31:0] out
);
localparam min = $signed(-128);
localparam max = $signed(127);


wire signed [31:0] mask;
wire signed [31:0] remainder;
wire signed [31:0] threshold;
wire signed [31:0] dividend;
wire signed [63:0] dot;
wire signed [31:0] result;

assign dot = $signed(acc) * $signed(qmul);
assign dividend = ($signed({dot[63:32], dot[31:0]}) + 32'sh40000000) >> 31;
assign mask = ~({32 {1'b1}} << shift);
assign remainder = dividend & mask;
assign threshold = (mask >> 1) + dividend[31];
assign result = (remainder > threshold) ? (($signed($signed(dividend) >>> shift) + 1'b1) + $signed(offset)) : ($signed($signed(dividend) >>> shift) + $signed(offset)) ;
assign out    = ($signed(result) < min) ? min :($signed(result) > max) ? max : result;

endmodule
*/

module quant 
(
  input  [31:0] acc,
  input  [31:0] dina,  
  input  [31:0] dinb,
  input         en,
  input         clk,
  output [31:0] out
);
localparam min = $signed(-128);
localparam max = $signed(127);

wire signed [31:0] qmul;
wire signed [3:0]  shift;
wire signed [31:0] mask;
wire signed [31:0] remainder;
wire signed [31:0] threshold;
wire signed [31:0] dividend;
wire signed [63:0] dot;
wire signed [31:0] result;

regn #(.DWIDTH(32)) qmulreg
(
  .din($signed(dina)),
  .dout(qmul),
  .en(en),
  .clr(1'b0),
  .clk(clk)
);

regn #(.DWIDTH(4)) shiftreg
(
  .din($signed(dinb[3:0])),
  .dout(shift),
  .en(en),
  .clr(1'b0),
  .clk(clk)
);

assign dot = $signed(acc) * $signed(qmul);
assign dividend = ($signed({dot[63:32], dot[31:0]}) + 32'sh40000000) >> 31;
assign mask = ~({32 {1'b1}} << shift);
assign remainder = dividend & mask;
assign threshold = (mask >> 1) + dividend[31];
assign result = (remainder > threshold) ? (($signed($signed(dividend) >>> shift) + 1'b1) + $signed(dina)) : ($signed($signed(dividend) >>> shift) + $signed(dina)) ;
assign out    = ($signed(result) < min) ? min :($signed(result) > max) ? max : result;

endmodule