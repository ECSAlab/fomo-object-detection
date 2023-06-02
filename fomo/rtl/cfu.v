`include "lib.v"
`include "mac.v"
`include "bias.v"
`include "quant.v"

module ctrl 
(
  input             cmd_valid,
  output            cmd_ready,
  input [9:0]       cmd_payload_function_id,
  output reg        rsp_valid,
  input             rsp_ready,
  output [3:0]      en,
  input             clk,
  input             reset
);

assign en[0] = cmd_valid & !cmd_payload_function_id[5] & !cmd_payload_function_id[4] &  cmd_payload_function_id[3];
assign en[1] = cmd_valid & !cmd_payload_function_id[5] &  cmd_payload_function_id[4] & !cmd_payload_function_id[3];
assign en[2] = cmd_valid & !cmd_payload_function_id[5] &  cmd_payload_function_id[4] &  cmd_payload_function_id[3];
assign en[3] = cmd_valid &  cmd_payload_function_id[5] & !cmd_payload_function_id[4] & !cmd_payload_function_id[3];

assign cmd_ready = ~rsp_valid;

always @(posedge clk) begin
  if (reset) begin
    rsp_valid <= 1'b0;
  end else if (rsp_valid) begin
    rsp_valid <= ~rsp_ready;
  end else if (cmd_valid) begin
    rsp_valid <= 1'b1;
  end
end

endmodule

module Cfu 
(
  input             cmd_valid,
  output            cmd_ready,
  input [9:0]       cmd_payload_function_id,
  input [31:0]      cmd_payload_inputs_0,
  input [31:0]      cmd_payload_inputs_1,
  output            rsp_valid,
  input             rsp_ready,
  output [31:0]     rsp_payload_outputs_0,
  input             clk,
  input             reset
);

//wire signed [8:0]  offset;
//wire signed [31:0] bias;
//wire signed [31:0] acc;
//wire signed [31:0] qmul;
//wire signed [3:0]  shift;
wire [3:0]  en;

wire [31:0] macout;
mac macunit 
(
  .data(cmd_payload_inputs_0),
  .weight(cmd_payload_inputs_1),
  //.acc(acc),
  //.offset(offset),
  .en({en[2],en[0]}),
  //.simd({cmd_payload_function_id[7],cmd_payload_function_id[6]}),
  .simd(cmd_payload_function_id[6]),
  .clk(clk), 
  .out(macout)
);

/*
mac macinst 
(
  .data(cmd_payload_inputs_0),
  .weight(cmd_payload_inputs_1),
  .acc(acc),
  .offset(offset),
  //en(en[2]), 
  .out(macout)
);

regn #(.DWIDTH(32)) accreg
(
  .din(macout),
  .dout(acc),
  .en(en[2]),
  .clr(en[0]),
  .clk(clk)
);

regn #(.DWIDTH(9)) offsetreg
(
  .din($signed(cmd_payload_inputs_0[8:0])),
  .dout(offset),
  .en(en[0]),
  .clr(1'b0),
  .clk(clk)
);
*/
wire [31:0] bmacout;

bias biasinst
(
  .dina(macout),
  //.dina(acc),
  .dinb(cmd_payload_inputs_1),
  .en(en[0]),
  .clk(clk),
  .out(bmacout)
);
/*
addn #(.DWIDTH(32)) biasinst
(
  .a(acc),
  .b(bias),
  .sum(bmacout)
);
regn #(.DWIDTH(32)) biasreg
(
  .din($signed(cmd_payload_inputs_1)),
  .dout(bias),
  .en(en[0]),
  .clr(1'b0),
  .clk(clk)
);
*/

wire [31:0] quantout;
quant quantinst
(
  .acc(bmacout),
  .dina($signed(cmd_payload_inputs_0)),
  .dinb($signed(cmd_payload_inputs_1)),
  //.offset($signed(cmd_payload_inputs_0)),
  .en(en[1]),
  .clk(clk),
  .out(quantout)
);
/*
quant quantinst
(
  .acc(bmacout),
  .qmul(qmul),
  .shift(shift),
  .offset($signed(cmd_payload_inputs_0)),
  //.en(en[3]),
  .out(quantout)
);
regn #(.DWIDTH(32)) qmulreg
(
  .din($signed(cmd_payload_inputs_0)),
  .dout(qmul),
  .en(en[1]),
  .clr(1'b0),
  .clk(clk)
);

regn #(.DWIDTH(4)) shiftreg
(
  .din($signed(cmd_payload_inputs_1[3:0])),
  .dout(shift),
  .en(en[1]),
  .clr(1'b0),
  .clk(clk)
);
*/
regn #(.DWIDTH(32)) out
(
  .din(quantout),
  .dout(rsp_payload_outputs_0),
  .en(en[3]),
  .clr(1'b0),
  .clk(clk)
);





ctrl controlunit 
(
  .cmd_valid(cmd_valid),
  .cmd_ready(cmd_ready),
  .cmd_payload_function_id(cmd_payload_function_id),
  .rsp_valid(rsp_valid),
  .rsp_ready(rsp_ready),
  .en(en),
  .clk(clk),
  .reset(reset)
);


/*
assign en[0] = cmd_valid & !cmd_payload_function_id[5] & !cmd_payload_function_id[4] &  cmd_payload_function_id[3];
assign en[1] = cmd_valid & !cmd_payload_function_id[5] &  cmd_payload_function_id[4] & !cmd_payload_function_id[3];
assign en[2] = cmd_valid & !cmd_payload_function_id[5] &  cmd_payload_function_id[4] &  cmd_payload_function_id[3];
assign en[3] = cmd_valid &  cmd_payload_function_id[5] & !cmd_payload_function_id[4] & !cmd_payload_function_id[3];


assign cmd_ready = ~rsp_valid;

always @(posedge clk) begin
  if (reset) begin
    rsp_valid <= 1'b0;
  end else if (rsp_valid) begin
    rsp_valid <= ~rsp_ready;
  end else if (cmd_valid) begin
    rsp_valid <= 1'b1;
    //rsp_payload_outputs_0 <= quantout;
    //rsp_payload_outputs_0 <= offset;
  end
end
*/

endmodule
