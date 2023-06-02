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
