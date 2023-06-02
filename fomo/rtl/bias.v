module bias 
(
  input [31:0]  dina,
  input [31:0]  dinb,
  input         en,
  input         clk,
  output [31:0] out
);


wire signed [31:0] bias;

addn #(.DWIDTH(32)) adder
(
  .a($signed(dina)),
  .b(bias),
  .sum(out)
);
regn #(.DWIDTH(32)) biasreg
(
  .din(dinb),
  .dout(bias),
  .en(en),
  .clr(1'b0),
  .clk(clk)
);


endmodule