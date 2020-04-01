// File:    ft601q_master_tx_tb.sv
// Author:  Lei Kuang
// Date:    31th of March 2020
// @ Imperial College London

module ft601q_master_tx_tb;

logic        ft_nrst;
logic        ft_clk;
logic        ft_rdy;
logic        wr_full;
logic        wr_en;
logic [31:0] wr_data;
logic        FT_CLK;
logic        FT_TXE_N;
logic        FT_RXF_N;
logic        FT_WR_N;
logic        FT_RD_N;
logic        FT_OE_N;
logic [3:0]  FT_BE;
logic [31:0] FT_DATA;
logic        FT_SIWU_N;

ft601q_master_tx dut(.*);

initial begin
    FT_CLK = '0;
    forever #5ns FT_CLK = ~FT_CLK;
end

assign FT_RXF_N = '1;

initial begin
    FT_TXE_N = '0;
    # 15us
    @(posedge ft_clk)
    FT_TXE_N <= '1;
    @(posedge ft_clk)
    FT_TXE_N <= '0;
end

initial begin
    ft_nrst = '0;
    @(posedge ft_clk)
    ft_nrst = '1;
end

initial begin
    wr_en   = '0;
    wr_data = '0;
    
    // Write 1KB data to FIFO
    @(posedge ft_rdy)
    wr_en <= '1;
    
    for(int i=0; i<256; i++) begin
        @(posedge ft_clk)
        wr_data <= wr_data + 32'd1;
    end
    
    @(posedge ft_clk) begin
    wr_en   <= '0;
    wr_data <= '0;
    end
    
    #5us
    // Write 2KB + 1 data to FIFO
    @(posedge ft_clk)
    wr_en <= '1;
    
    for(int i=0; i<1024; i++) begin
        @(posedge ft_clk)
        wr_data <= wr_data + 32'd1;
    end
    
    @(posedge ft_clk)
    wr_en <= '0;

end

endmodule
