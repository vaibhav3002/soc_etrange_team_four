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
   output  logic [3:0] p_wb_SEL_O,
   output  logic       p_wb_STB_O,
   output  logic       p_wb_WE_O, 

   // WB Signals for Slave
   input   [31:0]             p_wb_reg_DAT_I,
   output  logic [31:0] p_wb_reg_DAT_O,
   input   [31:0]       p_wb_reg_ADR_I,
   output  logic        p_wb_reg_ACK_O,
   input                p_wb_reg_CYC_I,
   output logic         p_wb_reg_ERR_O,
   input                p_wb_reg_LOCK_I,
   output logic         p_wb_reg_RTY_O,
   input   [3:0]             p_wb_reg_SEL_I,
   input                p_wb_reg_STB_I,
   input                p_wb_reg_WE_I 
 

   );

   //-----------Internal signals -------------------
   logic [7:0] 	       fifo[`VIDEO_IN_WINDOW_SIZE-1:0];
   logic [7:0] 	       fifo_counter, write_counter, block_offset;	
   logic [31:0]        address,start_address, module_register;
   bit		       initiliazed;
   bit		       go; // Indicates that a block of pixels have been loaded into fifo and are available to be written on ram.
   logic raise_irq;
   bit 			new_image;

   integer 		buffer_count;


   typedef enum        { waitForRamAddress=0,waitForValidFrame, waitForBufferToBeFilled, configureWbSignalsForBlkWrite, waitForWbAcknowledgement} VideoIN_States;
   VideoIN_States video_in_state;

// Module Instantiation

   wb_soc_slave reg0(p_clk_100mhz, 
		     p_resetn, 
                     raise_irq,
                     irq,
                     module_register,
                     initiliazed, 
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


   always_ff @ (posedge p_clk or negedge p_resetn)
     begin 
	if(!p_resetn)
	  begin 
	     fifo_counter <= 0;
	     go <= 1'b0;
	     buffer_count = 0;
	  end else
	    begin
	    if(new_image)
	    begin
	       if(frame_valid)
		 begin
		    if(line_valid)
		      begin
			 fifo[fifo_counter] <= pixel_in;

			 // activate the "go" signal if the block has been written to fifo
			 if(fifo_counter)
			   begin
			      if(!(fifo_counter % (`BLOCK_SIZE - 1)))
				begin
				   go <= 1'b1;
				   block_offset <= ((fifo_counter/(`BLOCK_SIZE - 1)) - 1)*8'd`BLOCK_SIZE;
				   buffer_count++;
				end
			   end		
			 
			 //reset fifo counter when it reaches window size
			 fifo_counter <= (fifo_counter == `VIDEO_IN_WINDOW_SIZE-1)?0:fifo_counter + 1;
		      end
		 end	
		end
	    end
     end

   always_ff @( posedge p_clk_100mhz or negedge p_resetn)
     begin

	if(!p_resetn)	
	  begin
	     address <= 32'h41000000; // Fixed Starting Address 
	     start_address <= 32'h41000000; // Fixed Starting Address 
	     video_in_state <= waitForRamAddress;
	     write_counter <= 8'h00;
	    // start_loading <= 1'b0;
	     // Clean Wb Signals

	     p_wb_STB_O  <= 1'b0;
	     p_wb_CYC_O  <= 1'b0;
	     p_wb_LOCK_O <= 1'b0;
	     p_wb_WE_O   <= 1'b0;
	     p_wb_ADR_O  <= 32'h00000000;
	     p_wb_DAT_O  <= 32'h00000000;
	     p_wb_SEL_O  <= 4'hF;
	     new_image <= 1'b0;

	  end
	else
	  begin
	     case (video_in_state)

	       waitForRamAddress:
		if(initiliazed && !frame_valid)
		begin
		video_in_state <= waitForValidFrame;
		start_address <= module_register;
	        address <= module_register;
		end

	
		waitForValidFrame:
		if(frame_valid)	
		begin
		new_image <= 1'b1;
		video_in_state <= waitForBufferToBeFilled;
		end
 
	       waitForBufferToBeFilled: 
		 begin
		    // wait for the fifo counter to be equal to block size
		    if(go)
		      begin
			 // Change to next state
			 go <= 1'b0;
			 video_in_state <= configureWbSignalsForBlkWrite;
			 write_counter <= 8'h00;
		      end
		 end 
	       configureWbSignalsForBlkWrite:
		 begin
		    // Read data from fifo buffer and write appropriate wb signals to start block write
		    p_wb_DAT_O <= (fifo[block_offset + write_counter] | fifo[block_offset + write_counter + 8'd1] << 8 | fifo[block_offset + write_counter + 8'd2] << 16 | fifo[block_offset + write_counter + 8'd3] << 24);	 
		    p_wb_ADR_O <= address;
		    p_wb_SEL_O <= 4'hF;	
		    p_wb_STB_O <= 1'b1;
		    p_wb_CYC_O <= 1'b1;
		    p_wb_WE_O <= 1'b1;
		    address <= address + 4;
		    write_counter <= write_counter + 4;
		    // Change to state where we wait for acknowledgement
		    video_in_state <= waitForWbAcknowledgement;
		 end
	       
	       waitForWbAcknowledgement:
		 begin
		    if (p_wb_ERR_I)
		      begin
			 // translate pragma off
			 $display(" Video-In: Error in WB transaction Cycle\n");
			 // translate pragma on
		      end
		    if(p_wb_ACK_I)
		      begin
			 // Received Acknowledgement. 
/*
			 if(buffer_count == 16)
			   start_loading <= 1;
*/
			// Request new address
      if ((write_counter == `BLOCK_SIZE - 4) && (address - start_address)>= 32'h0004b000 - 32'h00000004) 
			raise_irq <= 1'b1;


			 if(write_counter == `BLOCK_SIZE)	
			   begin
			      // Block has been written. 

			      // Check for address overflow or request for new start address once the image is complete 
			      if ((address > `RAM_BASE + `RAM_SIZE -(`BLOCK_SIZE)) || (address - start_address)>= 32'h0004b000) 
				begin
				   address <= module_register; 
				   start_address <= module_register;
				   raise_irq <= 1'b0;
				end

			      // Clean Wb Signals

			      p_wb_STB_O  <= 1'b0;
			      p_wb_CYC_O  <= 1'b0;
			      p_wb_LOCK_O <= 1'b0;
			      p_wb_WE_O   <= 1'b0;
			      p_wb_ADR_O  <= 32'hBadeC0de;
			      p_wb_DAT_O  <= 32'hDeadBeef;
			      p_wb_SEL_O  <= 4'hf;


			      video_in_state <= waitForBufferToBeFilled;	

			   end else
			     begin
				video_in_state <= configureWbSignalsForBlkWrite;
			     end // write_counter == `BLOCK_SIZE	

		      end // p_wb_ACK_I
		    
		 end // waitForWbAcknowledgement

	     endcase // case block

	  end // reset block
     end // always_ff


endmodule
