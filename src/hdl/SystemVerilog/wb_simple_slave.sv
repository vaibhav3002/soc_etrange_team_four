module wb_simple_slave (
                        input  p_clk,
                        input  p_resetn,
                        //  WB signals
                        input  [31:0] p_wb_DAT_I,
                        output [31:0] p_wb_DAT_O,
                        input  [31:0] p_wb_ADR_I,
                        output p_wb_ACK_O,
                        input  p_wb_CYC_I,
                        output p_wb_ERR_O,
                        input  p_wb_LOCK_I,
                        output p_wb_RTY_O,
                        input  [3:0]p_wb_SEL_I,
                        input  p_wb_STB_I,
                        input  p_wb_WE_I 
                        );

integer cycle;
integer w_req_cpt;


// always acknowledge requests
assign p_wb_ACK_O = p_wb_CYC_I && p_wb_STB_I;
// read write request counter
assign p_wb_DAT_O = w_req_cpt ;


always_ff @(posedge p_clk)
begin : transition
    if (!p_resetn)
    begin
    cycle = 0;
    w_req_cpt = 0;
    end
    else
    cycle++;
    if (p_wb_STB_I ) 
    begin
        if (p_wb_CYC_I ) 
        begin
            if (p_wb_WE_I) 
            begin
                w_req_cpt++;
//pragma translate_off
        $display ("SLAVE recieved Write REQ 0x%08X @cycle %d",p_wb_DAT_I,cycle);
//pragma translate_on
            end
            else
            begin
//pragma translate_off
        $display ("SLAVE recieved Read request @cycle %d",cycle);
//pragma translate_on
            end
        end
        else 
        begin
//pragma translate_off
        $display ("SLAVE recieved invalid STROBE @ cycle %d",cycle);
//pragma translate_on
        end
    end
end : transition

endmodule
