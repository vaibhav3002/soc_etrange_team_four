/*
 * =============================================================================
 *
 *       Filename:  wb_master_module.sv
 *
 *    Description:  generic wb master module (non-synthesizable)
 *
 *         Author:  Vaibhav Singh, vsingh@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *	  Note:	    This module can be used in a Wishbone master test bench
 * =============================================================================
 */


module wb_master_module (
		input                 p_clk,
		input                 p_resetn,
		//  WB signals
		input   [31:0]       p_wb_DAT_I,
		output  logic [31:0] p_wb_DAT_O,
		output  logic [31:0] p_wb_ADR_O,
		input                p_wb_ACK_I,
		output  logic        p_wb_CYC_O,
		input                p_wb_ERR_I,
		output  logic        p_wb_LOCK_O,
		input                p_wb_RTY_I,
		output  logic [3:0] p_wb_SEL_O,
		output  logic       p_wb_STB_O,
		output  logic       p_wb_WE_O 
                        );

////////////////////////////////////////////////////////////////////////////
// single read/write operation
////////////////////////////////////////////////////////////////////////////

task wb_read_at; // Single Read
input [31:0] addr;
output [31:0] tmp;
@(negedge p_clk);
p_wb_ADR_O = addr;
p_wb_SEL_O = 15;
p_wb_STB_O = 1;
p_wb_CYC_O = 1;
p_wb_WE_O  = 0;

WaitWbAck();

tmp = p_wb_DAT_I;

@(negedge p_clk);

CleanWb();

endtask // Single Read


task wb_write_at; // Single Write
input [31:0] addr;
input [3:0]  mask;
input [31:0] data;

@(negedge p_clk);
p_wb_ADR_O = addr;
p_wb_DAT_O = data;
p_wb_SEL_O = mask;
p_wb_STB_O = 1;
p_wb_CYC_O = 1;
p_wb_WE_O  = 1;

WaitWbAck();

@(negedge p_clk);

CleanWb();


endtask // Single Write

////////////////////////////////////////////////////////////////////////////
// burst read/write operation to/from an uint32_t array
////////////////////////////////////////////////////////////////////////////

task wb_read_blk; // Block Read
input [31:0] addr;
input data_finish; // To Signify when to stop reading the data
output [31:0] tmp;
@(negedge p_clk);
p_wb_ADR_O = addr;
p_wb_SEL_O = 'hf;
p_wb_STB_O = 1;
p_wb_CYC_O = 1;
p_wb_WE_O  = 0;

WaitWbAck();

tmp = p_wb_DAT_I;

if(data_finish)
begin
@(negedge p_clk);

CleanWb();
end
endtask // Block Read

task wb_write_blk; // Block Write
input [31:0] addr;
input [3:0]  mask;
input [31:0] data;
input  data_finish;

@(negedge p_clk);
p_wb_ADR_O = addr;
p_wb_DAT_O = data;
p_wb_SEL_O = mask;
p_wb_STB_O = 1;
p_wb_CYC_O = 1;
p_wb_WE_O  = 1;

WaitWbAck();

if(data_finish)
begin
@(negedge p_clk);
CleanWb();
end

endtask // Block Write


task WaitWbAck; // Wait for Acknowledgement Signal

@(posedge p_clk);

do begin 
 	if(p_wb_ERR_I)
	begin
	// pragma translate_off
	$display(" Error in WB transaction\n");
	// pragma translate_on
	end
end while (!p_wb_ACK_I); 

endtask // WaitWbAck

task CleanWb; // Resets Wishbone Signals
p_wb_STB_O  = 0;
p_wb_CYC_O  = 0;
p_wb_LOCK_O = 0;
p_wb_WE_O   = 0;
p_wb_ADR_O  = 'hBadeC0de;
p_wb_DAT_O  = 'hDeadBeef;
p_wb_SEL_O  = 0;
endtask //CleanWb


always_ff @ (posedge p_clk)
begin
if (!p_resetn)
	CleanWb();
end


endmodule
