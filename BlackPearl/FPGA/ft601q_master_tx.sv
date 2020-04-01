// File:    ft601q_master_tx.sv
// Author:  Lei Kuang
// Date:    31th of March 2020
// @ Imperial College London

// FIFO Master for FT601Q
// - for 245 synchronous FIFO mode protocols
// - !!! TX ONLY (Write Only)
// - !!! Minimum data size of 1KB for each transaction

module ft601q_master_tx
(
    input  logic        ft_nrst,    // System reset
    output logic        ft_clk,     // System clock
    output logic        ft_rdy,     // System ready
        
    // User Interface
    output logic        wr_full,    // FIFO write full
    input  logic        wr_en,      // FIFO write enable
    input  logic [31:0] wr_data,    // FIFO write data

    // FT601Q Physical Port
    input  logic        FT_CLK,     // FT601Q FIFO bus clock
    input  logic        FT_TXE_N,   // FT601Q FIFO write acknowledge
    input  logic        FT_RXF_N,   // FT601Q FIFO read data available
    output logic        FT_WR_N,    // FT601Q FIFO write enable
    output logic        FT_RD_N,    // FT601Q FIFO read enable
    output logic        FT_OE_N,    // FT601Q FIFO output enable (data bus direction)
    output logic [3:0]  FT_BE,      // FT601Q FIFO bus byte enable
    output logic [31:0] FT_DATA,    // FT601Q FIFO data
    // - Reserved
    output logic        FT_SIWU_N   // Externel pull up
);

// ----------------------------------------------------------------
// FT601Q Physical Port Translation
// ----------------------------------------------------------------
assign FT_RD_N   = '1;  // Never read
assign FT_OE_N   = '1;  // Always input
assign FT_BE     = '1;  // Always 4 bytes
assign FT_SIWU_N = '1;

logic        ft_wr_ack;
logic        ft_wr_en;
logic [31:0] ft_wr_data;

assign ft_clk    = FT_CLK;
assign ft_wr_ack = ~FT_TXE_N;
assign FT_WR_N   = ~ft_wr_en;
assign FT_DATA   = ft_wr_data;

// ----------------------------------------------------------------
// FIFO 256x32b (1KB)
// ----------------------------------------------------------------
logic           rd_en;
logic           rd_empty;
logic           rd_valid;
logic [31:0]    rd_data;
logic [7:0]     rd_dcnt;

fifo_256x32 fifo_256x32_inst
(
    .nrst  (ft_nrst),
    .clk   (ft_clk),
    .rdy   (ft_rdy),
    
    .full  (wr_full),
    .wr_en (wr_en),
    .din   (wr_data),
    
    .empty (rd_empty),
    .rd_en (rd_en),
    .dout  (rd_data),
    .dcnt  (rd_dcnt)
);

// ----------------------------------------------------------------
// FT601Q Write Control
// ----------------------------------------------------------------
// rd_dcnt:     how many data available in fifo
// trans_cnt:   how many data left to read out

enum {rst, init, buff, trans} w_state;

logic [7:0] trans_cnt;

always_ff @ (posedge ft_clk, negedge ft_nrst) begin
    if(~ft_nrst) begin
        w_state   <= rst;
        trans_cnt <= '0;
    end
    else
        case(w_state)
            rst:    if(ft_rdy)
                        w_state <= init;
            init:   if(~rd_empty)                                   // At initial, no matter ft601q is ready or not,    
                        w_state <= buff;                            // - always read data if not empty, so that once it is ready, 
                                                                    // - no additional clock cycle is required to read data (ensure lowest latency)
            buff:   if((rd_dcnt=='1 | wr_full) & ft_wr_ack) begin   // Keep buffering data until dcnt reaches 255 (255 + 1 -> 1KB)              
                        w_state   <= trans;                         // - if ft601q is ready to accept data, go to trans state
                        trans_cnt <= 8'd254;                        // - !!! only 254 data left for 1KB transaction as buff state also reads out one
                    end
            trans:  if(trans_cnt!='0)                               // Burst transaction of 256x4B (1KB)
                        trans_cnt <= trans_cnt - 8'd1;  
                    else
                        if(~rd_empty)                               // After 1KB data have been read out
                            w_state <= buff;                        // - if fifo is not empty, read data and go to buff state
                        else
                            w_state <= init;                        // - if fifo is empty, go to init state
        endcase
end

always_comb begin
    rd_en    = '0;
    rd_valid = '0;
    case(w_state)
        init:   rd_en    = ~rd_empty;
        buff:   begin
                rd_en    = (rd_dcnt=='1 | wr_full) & ft_wr_ack;
                rd_valid = (rd_dcnt=='1 | wr_full) & ft_wr_ack;
                end
        trans:  begin
                rd_en    = ((trans_cnt!='0) | ~rd_empty) & ft_wr_ack ;
                rd_valid = '1;
                end
    endcase
end

assign ft_wr_en   = rd_valid;
assign ft_wr_data = rd_data;

// ----------------------------------------------------------------
// Internal Logic Analyzer
// ----------------------------------------------------------------
ila_ft601q ila_inst
(
    .clk     (ft_clk),
    .probe0  (ft_nrst),
    .probe1  (ft_rdy),
    .probe2  (wr_full),
    .probe3  (wr_en),
    .probe4  (wr_data[31:0]),
    .probe5  (rd_dcnt[7:0]),
    .probe6  (rd_en),
    .probe7  (rd_empty),
    .probe8  (rd_valid),
    .probe9  (rd_data[31:0]),
    .probe10 (ft_wr_ack),
    .probe11 (FT_TXE_N),
    .probe12 (FT_RXF_N),
    .probe13 (FT_WR_N),
    .probe14 (FT_RD_N),
    .probe15 (FT_OE_N),
    .probe16 (FT_BE[3:0])
);

endmodule
