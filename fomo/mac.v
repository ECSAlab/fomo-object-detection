
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
assign  d1 = (simd == 1'b0) ? 8'h00 : $signed(data[15 : 8]);
assign  d2 = (simd == 1'b0) ? 8'h00 : $signed(data[23 : 16]);
assign  d3 = (simd == 1'b0) ? 8'h00 : $signed(data[31 : 24]);

assign  w0 = $signed(weight[7 : 0]);
assign  w1 = (simd == 1'b0) ? 8'h00 : $signed(weight[15 : 8] );
assign  w2 = (simd == 1'b0) ? 8'h00 : $signed(weight[23 : 16]);
assign  w3 = (simd == 1'b0) ? 8'h00 : $signed(weight[31 : 24]);

assign  mul0 = (d0 + $signed(offset)) * w0;
assign  mul1 = (d1 + $signed(offset)) * w1;
assign  mul2 = (d2 + $signed(offset)) * w2;
assign  mul3 = (d3 + $signed(offset)) * w3;

assign  prod =  $signed(acc) + mul0 + mul1 + mul2 + mul3;
assign  out =  $signed(acc);


endmodule