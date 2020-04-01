// File:    fifo_256x32_tb.sv
// Author:  Lei Kuang
// Date:    31th of March 2020
// @ Imperial College London

module fifo_256x32_tb;

logic        nrst;
logic        clk;
logic        rdy;
    
logic        wr_en;
logic [31:0] din;
logic        full;
logic        rd_en;
logic [31:0] dout;
logic        empty;
logic [7:0]  dcnt;

fifo_256x32 dut(.*);

initial begin
    clk = '0;
    forever #5ns clk = ~clk;
end

initial begin
    nrst  = '0;
    @(posedge clk)
    nrst = '1;
end

assign wr_en = ~full & rdy;
assign rd_en = full;

always_ff @ (posedge clk, negedge nrst) begin
    if(~nrst)
        din <= '0;
    else
        if(wr_en)
            din <= din + 32'd1;
end

endmodule
