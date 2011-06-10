/*
 * =============================================================================
 *
 *       Filename:  video_in.sv
 *
 *    Description:  Video IN - Type WB master
 *                  This module takes the pixels from video generator 
 *                  and stores on to ram.
 *
 *         Author:  Vaibhav SINGH
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

`define VIDEO_IN_WINDOW_SIZE  64
`define BLOCK_SIZE   32
`define RAM_BASE     32'h40000000
`define RAM_SIZE     32'h02000000

`define BLOCK_MODULO  `VIDEO_IN_WINDOW_SIZE/`BLOCK_SIZE

module video_in 
  (
   input  p_clk, // 25 Mhz clock
   input  p_resetn, 
   input  p_clk_100mhz,

   // Interface signals with Video Gen

   input  line_valid,
   input  frame_valid,
   input  byte unsigned pixel_in,

   // indicates video out to start loading. Need to remove this and raise an interruption to lm32 instead.
   output logic start_loading, 

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

   //-----------Internal signals -------------------
   logic [7:0]        fifo[`VIDEO_IN_WINDOW_SIZE-1:0];
   logic [7:0]	      fifo_counter;	
   integer            read_count, write_count;
   bit                reset_done;
   logic [31:0]        address;
   logic [31:0]        initial_image_position;
   logic            data_finish;

   // TODO!!

   typedef enum { waitForBufferToBeFilled = 0, configureWbSignalsForBlkWrite, waitForWbAck} VideoIN_States;
   VideoIN_States video_in_state;


always_ff @ (posedge p_clk or negedge p_resetn)
begin 
if(!p_resetn)
begin 
	fifo_counter <= 0;
end else
begin
	if(frame_valid)
	begin
	  if(line_valid)
	    begin
		fifo[fifo_counter] <= pixel_in;
		fifo_counter <= (fifo_counter == `VIDEO_IN_WINDOW_SIZE-1)?0:fifo_counter + 1;
	    end
	end	
end
end

always_ff @( posedge p_clk_100mhz or negedge p_resetn)
begin

	if(!p_resetn)	
	begin
	address <= 32'h00000000;
	video_in_state <= waitForBufferToBeFilled;
	end
	else
	begin 


	end
end


endmodule
