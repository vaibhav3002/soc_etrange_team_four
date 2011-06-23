/*
 * =============================================================================
 *
 *       Filename:  video_out.sv
 *
 *    Description:  Video OUT - Type WB master
 *                  This module takes the pixels from the RAM and sends them
 *                  to the video display 
 *
 *         Author:  Thibault Porteboeuf
 *         			Vaibhav SINGH
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

`define VIDEO_OUT_WINDOW_SIZE  64
`define BLOCK_SIZE   32
`define RAM_BASE     32'h40000000
`define RAM_SIZE     32'h02000000

`define IMAGE_HEIGHT 480
`define IMAGE_WIDTH  640
`define LINE_SYNC    160
`define FRAME_SYNC   40

`define BLOCK_MODULO  `VIDEO_OUT_WINDOW_SIZE/`BLOCK_SIZE

module video_out
  (
   input  p_clk, // 25 Mhz clock
   input  p_resetn, 
   input  p_clk_100mhz,

   // Interface signals with Video Display
   output logic line_valid,
   output logic frame_valid,
   output  byte unsigned pixel_out,
   output logic irq,

   //  WB signals for Master
   input   [31:0]       p_wb_DAT_I,
   output  logic [31:0] p_wb_DAT_O,
   output  logic [31:0] p_wb_ADR_O,
   input                p_wb_ACK_I,
   output  logic        p_wb_CYC_O,
   input                p_wb_ERR_I,
   output  logic        p_wb_LOCK_O,
   input                p_wb_RTY_I,
   output  logic [3:0]  p_wb_SEL_O,
   output  logic        p_wb_STB_O,
   output  logic        p_wb_WE_O, 

   // WB Signals for Slave
   input   [31:0]       p_wb_reg_DAT_I,
   output  logic [31:0] p_wb_reg_DAT_O,
   input   [31:0]       p_wb_reg_ADR_I,
   output  logic        p_wb_reg_ACK_O,
   input                p_wb_reg_CYC_I,
   output logic         p_wb_reg_ERR_O,
   input                p_wb_reg_LOCK_I,
   output logic         p_wb_reg_RTY_O,
   input   [3:0]        p_wb_reg_SEL_I,
   input                p_wb_reg_STB_I,
   input                p_wb_reg_WE_I 
 

   );

   //-----------Internal signals -------------------
   logic [7:0] 	       fifo[`VIDEO_OUT_WINDOW_SIZE-1:0];
   logic [7:0] 	       fifo_counter, read_counter, block_offset;	
   logic [31:0]        address,start_address, module_register;
   bit		           go, go_ack; // Indicates that a block of pixels have been loaded into fifo and are available
   logic 			   raise_irq;
   bit 				   start;
   logic 	   	 	   prev_frame_valid;
   logic [9:0] 	   	   pix_counter; //Counts up to 1023, required: img width+hsync=640+160=800
   logic [9:0]		   line_counter;//Counts up to 1023, required: img height+vsync=480+40=520

   typedef enum        { waitForRamAddress=0,initialize,waitForGo,initReadSequence,waitForAck } VideoOUT_States;
   VideoOUT_States video_out_state;

// Module Instantiation

   wb_soc_slave reg0(p_clk_100mhz, 
		     p_resetn, 
             raise_irq,
             irq,
			 module_register,
			 initialized,
			 written,
			 p_wb_reg_DAT_I,
		     p_wb_reg_DAT_O,
		     p_wb_reg_ADR_I,
		     p_wb_reg_ACK_O,
		     p_wb_reg_CYC_I,
		     p_wb_reg_ERR_O,
			 p_wb_reg_LOCK_I,
		     p_wb_reg_RTY_O,
		     p_wb_reg_SEL_I,
		     p_wb_reg_STB_I,
		     p_wb_reg_WE_I	
   	 );

	assign p_wb_DAT_O = 32'h00000000;

	//This block generates the proper video signals. It will start once the
	//buffer has been first initialized, and won't stop until a reset signal
	//has been received.
	always_ff @ (posedge p_clk or negedge p_resetn)
	begin 
		if((!p_resetn))
		begin 
			fifo_counter <= 0;
			pix_counter <= 0;
			frame_valid <= 0;
			line_counter <= 0;
			pixel_out <= 0;
			go <= 0;
		end else
	    begin
		    if(start)//The buffer has been initialized with pixels
		    begin

			if( line_counter < `IMAGE_HEIGHT && (pix_counter > `LINE_SYNC - 1))
				begin
						pixel_out <= fifo[fifo_counter];
						if(!((fifo_counter +1)% (`BLOCK_SIZE)))
						begin
				 			//The module has reached the end of a block, it
							//should now ask for a new one to be preloaded in
							//the buffer
							go <= 1'b1;
						end
						fifo_counter <= (fifo_counter == `VIDEO_OUT_WINDOW_SIZE-1)?0:fifo_counter + 1; //We increment fifo_counter modulo the window size
				end else
				
						pixel_out <= 8'hbb;
						line_valid <= (line_counter < `IMAGE_HEIGHT && pix_counter > `LINE_SYNC - 1) ? 1'b1 : 1'b0; 	
						frame_valid <= (line_counter < `IMAGE_HEIGHT ) ? 1'b1 : 1'b0; 	
						if (pix_counter == `IMAGE_WIDTH+`LINE_SYNC-1)
						begin
						line_counter <= line_counter + 1;
						pix_counter <= 0;
						end else 
							pix_counter <= pix_counter + 1;
						
						if (pix_counter == `IMAGE_WIDTH+`LINE_SYNC-1)
						begin
						line_counter <= line_counter + 1;
						pix_counter <= 0;
						end else 
							pix_counter <= pix_counter + 1;
						
						if (line_counter == `IMAGE_HEIGHT+`FRAME_SYNC)
						begin
						line_counter <= 0;
						pix_counter <= 0;
						end  
			
				if (go_ack)
					go <= 1'b0;
/*
				if (line_counter<`IMAGE_HEIGHT)
				begin
					frame_valid <= 1'b1;
					if (pix_counter<`IMAGE_WIDTH)
					begin
						line_valid <= 1;
						pixel_out <= fifo[fifo_counter];
						if(!((fifo_counter +1)% (`BLOCK_SIZE)))
						begin
				 			//The module has reached the end of a block, it
							//should now ask for a new one to be preloaded in
							//the buffer
							go <= 1'b1;
						end
	
						fifo_counter <= (fifo_counter == `VIDEO_OUT_WINDOW_SIZE-1)?0:fifo_counter + 1; //We increment fifo_counter modulo the window size
					end else if (pix_counter<`IMAGE_WIDTH+`LINE_SYNC)
						line_valid <= 0;
				end else if (line_counter<`IMAGE_HEIGHT+`FRAME_SYNC)
				begin
					frame_valid <= 0;
					if (frame_valid) //Falling edge
						raise_irq <= 1'b1;
					else
						raise_irq <= 1'b0;
					
				end
				
				if (pix_counter == `IMAGE_WIDTH+`LINE_SYNC-1)
				begin	
					line_counter <= (line_counter == `IMAGE_HEIGHT+`FRAME_SYNC - 1)?0:line_counter +1;
					pix_counter <= 0;
				end else
					pix_counter <= pix_counter+1;
				if (go_ack)
					go <= 1'b0;
*/
			end
			else begin
				fifo_counter <= 0;
				pix_counter <= 0;
				line_counter <= 0;
				frame_valid <= 0;
				go <= 0;
			end	
		end
	end


    always_ff @( posedge p_clk_100mhz or negedge p_resetn)
	begin
		if(!p_resetn)	
		begin
			address <= 32'h41000000; // Fixed Starting Address 
			start_address <= 32'h41000000; // Fixed Starting Address 
			video_out_state <= waitForRamAddress;
			read_counter <= 8'h00;
			// Clean Wb Signals
			p_wb_STB_O  <= 1'b0;
			p_wb_CYC_O  <= 1'b0;
			p_wb_LOCK_O <= 1'b0;
			p_wb_WE_O   <= 1'b0;
			p_wb_ADR_O  <= 32'h00000000;
			p_wb_SEL_O  <= 4'hF;
			start <= 1'b0;

		end else
		begin
			case (video_out_state)
				waitForRamAddress:
					if (initialized&&written)
					begin
						video_out_state <= initialize;
						start_address <= module_register;
						address <= module_register;
						read_counter <= 0;
						block_offset <= 0;
						start <= 1'b0;
					end

				waitForGo:
					begin
						p_wb_STB_O <= 1'b0;
						p_wb_CYC_O <= 1'b0;
						p_wb_LOCK_O <= 1'b0;
						p_wb_WE_O <= 1'b0;
						p_wb_ADR_O <= 32'hBadeC0de;
						p_wb_SEL_O <= 4'hf;

						if (!written)
						begin
							video_out_state <= waitForRamAddress;
						end else
						if (!go && go_ack) //The other block received the go_ack signal
							go_ack <= 1'b0;
						else if (go && !go_ack) //No acknowledgement was send, and go is high -> The other block requires new data
						begin
							if (read_counter == `BLOCK_SIZE) //A whole block was read
							begin
								go_ack <= 1'b1; //Acknowledge go
								read_counter <= 0;
								block_offset <= (block_offset)?0:`BLOCK_SIZE;
							end else // Reading
								video_out_state <= initReadSequence;
						end
					end

				initialize:
					begin
						p_wb_STB_O <= 1'b0;
						p_wb_CYC_O <= 1'b0;
						p_wb_LOCK_O <= 1'b0;
						p_wb_WE_O <= 1'b0;
						p_wb_ADR_O <= 32'hBadeC0de;
						p_wb_SEL_O <= 4'hf;
	
						if (`VIDEO_OUT_WINDOW_SIZE == read_counter + block_offset)
						begin
							read_counter <= 0;
							block_offset <= 0;
							start <= 1'b1;
							video_out_state <= waitForGo;
						end else
						begin
							if (read_counter == `BLOCK_SIZE)
							begin
								read_counter <= 0;
								block_offset <= `BLOCK_SIZE;
							end
							video_out_state <= initReadSequence;
						end
					end

				initReadSequence:
					begin
						p_wb_ADR_O <= address;
						p_wb_SEL_O <= 4'hF;
						p_wb_STB_O <= 1'b1;
						p_wb_CYC_O <= 1'b1;
						p_wb_WE_O <= 1'b0;
						address <= address + 4;
						read_counter <= read_counter + 4;
						video_out_state <= waitForAck;
					end

				waitForAck:
					begin
						if (p_wb_ERR_I)
						begin
							//translate pragma off
							$display(" Video-Out: Error in WB transaction cycle\n");
							//translate pragma on
						end
						if (p_wb_ACK_I)
						begin
							//received acknowledgement
							fifo[read_counter+block_offset-4] <= p_wb_DAT_I[7:0];
							fifo[read_counter+block_offset-3] <= p_wb_DAT_I[15:8];
							fifo[read_counter+block_offset-2] <= p_wb_DAT_I[23:16];
							fifo[read_counter+block_offset-1] <= p_wb_DAT_I[31:24];

					if ((address - start_address)== (32'h0004b000 - 32'h00000004))
                                               raise_irq <= 1'b1;
							
							if(raise_irq)
								raise_irq <= 0;
							if (read_counter == `BLOCK_SIZE)
							begin
								//The buffer has been filled

//								if((address - start_address)>= 32'h0004b000)
//									raise_irq <= 1'b1;


								if (start)
									video_out_state <= waitForGo;
								else
									video_out_state <= initialize;
	
							end else
							begin
								p_wb_ADR_O <= address;
								p_wb_SEL_O <= 4'hF;
								p_wb_STB_O <= 1'b1;
								p_wb_CYC_O <= 1'b1;
								p_wb_WE_O <= 1'b0;
								address <= address + 4;
								read_counter <= read_counter + 4;
							end
						end
					end
			endcase
		end
	end // always_ff


endmodule
