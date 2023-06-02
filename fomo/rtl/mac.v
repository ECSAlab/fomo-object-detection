
/*
module mux
#(parameter DWIDTH = 8) 
(
  input      [DWIDTH-1:0] ina,
  input      [DWIDTH-1:0] inb,
  output     [DWIDTH-1:0] dout,
  input                   sel
);

  assign dout = (sel) ? ina : inb;
endmodule

module regn
#(parameter DWIDTH = 8) 
(
  input      [DWIDTH-1:0] din,
  output reg [DWIDTH-1:0] dout,
  input                   en,
  input                   clr,
  input                   clk
);
always @(posedge clk) 
  if (clr)
    dout <= 0;
  else if (en)
    dout <= din;

endmodule

module addn 
#(parameter DWIDTH = 8) 
(
  input  [DWIDTH-1:0] a,
  input  [DWIDTH-1:0] b,
  output [DWIDTH-1:0] sum
);

	assign sum = $signed(a) + $signed(b);

endmodule

module multn 
#(parameter DWIDTH = 8) 
(
  input  [DWIDTH-1:0]   a,
  input  [DWIDTH-1:0]   b,
  output [2*DWIDTH-1:0] mul
);
    assign mul = $signed(a) * $signed(b);

endmodule
*/
/*
module mac 
(
  input      [31:0] data,
  input      [31:0] weight,
  input      [31:0] acc,
  input      [8:0]  offset,
  output     [31:0] out
);


wire signed [7:0] sum0, sum1, sum2, sum3; 
wire signed [15:0] mul0, mul1, mul2, mul3; 
////wire signed [15:0] tmp0, tmp1; 
wire signed [31:0] prod;
//wire signed [31:0] sum; 
//wire signed [31:0] isum; 

//addn #(.DWIDTH(16)) add0($signed(data[7 : 0])  ,$signed(offset),sum0);
//addn #(.DWIDTH(16)) add1($signed(data[15 : 8]) ,$signed(offset),sum1)
//addn #(.DWIDTH(16)) add2($signed(data[23 : 16]),$signed(offset),sum2);
//addn #(.DWIDTH(16)) add3($signed(data[31 : 24]),$signed(offset),sum3);

//multn #(.DWIDTH(16)) mult0($signed(weight[7 : 0])  ,sum0,mul0);
//multn #(.DWIDTH(16)) mult1($signed(weight[15 : 8]) ,sum1,mul1);
//multn #(.DWIDTH(16)) mult2($signed(weight[23 : 16]),sum2,mul2);
//multn #(.DWIDTH(16)) mult3($signed(weight[31 : 24]),sum3,mul3);

assign mul0 = ($signed(data[7 : 0])   + $signed(offset)) * $signed(weight[7 : 0]);
assign mul1 = ($signed(data[15 : 8])  + $signed(offset)) * $signed(weight[15 : 8]);
assign mul2 = ($signed(data[23 : 16]) + $signed(offset)) * $signed(weight[23 : 16]);
assign mul3 = ($signed(data[31 : 24]) + $signed(offset)) * $signed(weight[31 : 24]);
//adder tree -> out;
endmodule
*/
/*
module mac 
(
  input      [31:0] data,
  input      [31:0] weight,
  input      [31:0] acc,
  input      [8:0]  offset,
  //input             en,
  output reg [31:0] out
);

reg signed [15:0] mul0, mul1, mul2, mul3; 

always @(*) begin
  //if(en) begin
      mul0 = ($signed(data[7 : 0])   + $signed(offset)) * $signed(weight[7 : 0]);
      mul1 = ($signed(data[15 : 8])  + $signed(offset)) * $signed(weight[15 : 8]);
      mul2 = ($signed(data[23 : 16]) + $signed(offset)) * $signed(weight[23 : 16]);
      mul3 = ($signed(data[31 : 24]) + $signed(offset)) * $signed(weight[31 : 24]);
      out = $signed(acc) + mul0 + mul1 + mul2 + mul3;
  //end
end

endmodule



module mac 
(
  input      [31:0] data,
  input      [31:0] weight,
  input      [31:0] acc,
  input      [8:0]  offset,
  //input             en,
  output reg [31:0] out
);

reg signed [15:0] mul0, mul1, mul2, mul3; 

always @(*) begin
  //if(en) begin
      mul0 = ($signed(data[7 : 0])   + $signed(offset)) * $signed(weight[7 : 0]);
      mul1 = ($signed(data[15 : 8])  + $signed(offset)) * $signed(weight[15 : 8]);
      mul2 = ($signed(data[23 : 16]) + $signed(offset)) * $signed(weight[23 : 16]);
      mul3 = ($signed(data[31 : 24]) + $signed(offset)) * $signed(weight[31 : 24]);
      out = $signed(acc) + mul0 + mul1 + mul2 + mul3;
  //end
end

endmodule
*/

module mac 
(
  input [31:0]  data,
  input [31:0]  weight,
  input [1:0]   en,
  //input [1:0]   simd,
  input         simd,
  input         clk,
  output [31:0] out
);

wire signed [15:0] mul0, mul1, mul2, mul3; 
wire signed [7:0]  d0, d1, d2, d3; 
wire signed [7:0]  w0, w1, w2, w3; 
//wire signed [8:0]  o; 
wire signed [31:0] prod;
wire signed [8:0]  offset;
wire signed [31:0] acc;

regn #(.DWIDTH(32)) accreg
(
  .din(prod),
  .dout(acc),
  .en(en[1]),
  .clr(en[0]),
  .clk(clk)
);

regn #(.DWIDTH(9)) offsetreg
(
  .din($signed(data[8:0])),
  .dout(offset),
  .en(en[0]),
  .clr(1'b0),
  .clk(clk)
);


assign  d0 = $signed(data[7 : 0]);
//assign  d1 = (simd == 2'b01) ? 8'h00 :(simd == 2'b11) ? $signed(data[15 : 8])  : $signed(data[15 : 8] );
assign  d1 = (simd == 1'b0) ? 8'h00 : $signed(data[15 : 8]);
assign  d2 = (simd == 1'b0) ? 8'h00 : $signed(data[23 : 16]);
assign  d3 = (simd == 1'b0) ? 8'h00 : $signed(data[31 : 24]);

assign  w0 = $signed(weight[7 : 0]);
assign  w1 = (simd == 1'b0) ? 8'h00 : $signed(weight[15 : 8] );
assign  w2 = (simd == 1'b0) ? 8'h00 : $signed(weight[23 : 16]);
assign  w3 = (simd == 1'b0) ? 8'h00 : $signed(weight[31 : 24]);

//assign  o = (simd == 1'b0) ? 9'b000000000 : $signed(offset);
//assign  mul0 = (d0 + $signed(offset)) * w0;
//assign  mul1 = (d1 + o) * w1;
//assign  mul2 = (d2 + o) * w2;
//assign  mul3 = (d3 + 0) * w3;

assign  mul0 = (d0 + $signed(offset)) * w0;
assign  mul1 = (d1 + $signed(offset)) * w1;
assign  mul2 = (d2 + $signed(offset)) * w2;
assign  mul3 = (d3 + $signed(offset)) * w3;


//assign  mul0 = ($signed(data[7 : 0])   + $signed(offset)) * $signed(weight[7 : 0]);
//assign  mul1 = ($signed(data[15 : 8])  + $signed(offset)) * $signed(weight[15 : 8]);
//assign  mul2 = ($signed(data[23 : 16]) + $signed(offset)) * $signed(weight[23 : 16]);
//assign  mul3 = ($signed(data[31 : 24]) + $signed(offset)) * $signed(weight[31 : 24]);

assign  prod =  $signed(acc) + mul0 + mul1 + mul2 + mul3;
assign  out =  $signed(acc);


endmodule