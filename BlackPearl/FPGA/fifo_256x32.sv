// File:    fifo_256x32.sv
// Author:  Lei Kuang
// Date:    31th of March 2020
// @ Imperial College London

// 1K synchronous FIFO

module fifo_256x32
(
    input  logic        nrst,
    input  logic        clk,
    output logic        rdy,
    
    input  logic        wr_en,
    input  logic [31:0] din,
    output logic        full,
    input  logic        rd_en,
    output logic [31:0] dout,
    output logic        empty,
    output logic [7:0]  dcnt
);

logic rdy_lock;

fifo_256x32_gen fifo_256x32_inst
(
    .clk        (clk),
    .rst        (~nrst),
    .din        (din),
    .wr_en      (wr_en),
    .rd_en      (rd_en),
    .dout       (dout),
    .full       (full),
    .empty      (empty),
    .data_count (dcnt)
);

// After reset, full signal deasserts to indicate the end of reset
always_ff @ (posedge clk, negedge nrst) begin
    if(~nrst)
        rdy_lock <= '0;
    else
        if(~rdy_lock)
            rdy_lock <= ~full;
end

assign rdy = rdy_lock;

endmodule
