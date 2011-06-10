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

`define VIDEO_IN_WINDOW_SIZE  16
`define BLOCK_SIZE   8
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
   logic [31:0]        data[`VIDEO_IN_WINDOW_SIZE-1:0] ;
   integer            read_count, write_count;
   bit                reset_done;
   integer            index, index_8,buffer_count,i;
   logic [31:0]        mem;
   logic [31:0]        initial_image_position;
   logic            data_finish;

   // TODO!!
   // Instantiating master module
   wb_master_module master0(.p_clk(p_clk), .p_resetn(p_resetn), .p_wb_DAT_I(p_wb_DAT_I), .p_wb_DAT_O(p_wb_DAT_O), .p_wb_ADR_O(p_wb_ADR_O), .p_wb_ACK_I(p_wb_ACK_I), .p_wb_CYC_O(p_wb_CYC_O), .p_wb_ERR_I(p_wb_ERR_I), .p_wb_LOCK_O(p_wb_LOCK_O), .p_wb_RTY_I(p_wb_RTY_I), .p_wb_SEL_O(p_wb_SEL_O), .p_wb_STB_O(p_wb_STB_O), .p_wb_WE_O(p_wb_WE_O));


   always_ff @ (posedge p_clk)
     begin: load_to_buffer 
    if (!p_resetn)
      begin 
         reset_done <= '1;    
         write_count <= '0;
      end else if (reset_done)
        begin 

           if(frame_valid)
         begin 
            if(line_valid)
              begin
             data[index]=data[index]|(pixel_in<<8*(index_8 % 4));
                 data[index]=data[index]&(((~pixel_in)<<8*(index_8 % 4)));     
               index_8++;
             if ((index_8 % 4 ==0) && (index_8!=0))                                  
               index++;

             if (((index % `BLOCK_SIZE)==0) && (index!=0)&& (index_8 % 4==0) )  
               write_count <= (write_count + 1) % `BLOCK_MODULO;

             index = (index==`VIDEO_IN_WINDOW_SIZE) ? 0:index;                           
              end // Line valid
         end // Frame valid
        end // reset
     end // always_ff


   always_ff @ (posedge p_clk_100mhz)
     begin: load_buffer_to_ram 
    if (!p_resetn)
      begin 
         reset_done <= '1;    
         read_count <= '0;
         buffer_count = 0;
         mem = 'h41000000;
         initial_image_position <= 'h41000000;
      end else if (reset_done)
          begin
           if (read_count!=write_count)
         begin
            for(i =0; i < `BLOCK_SIZE; i++)
              begin 
             data_finish = (i < `BLOCK_SIZE -1) ? 0:1; 
             master0.wb_write_blk(mem,4'hF,data[(read_count*`BLOCK_SIZE)+ i],data_finish);
             mem = mem + 4;
              end
            mem = mem + (4*`BLOCK_SIZE);
            if ((mem>`RAM_BASE+`RAM_SIZE -4*(`BLOCK_SIZE)) || (mem-initial_image_position)>='h0004b000) 
              begin                                                                                           
             mem='h40005000;
             initial_image_position <= mem;
              end
            read_count <= (read_count+1) % `BLOCK_MODULO;                                      
            
            buffer_count++;

            if (buffer_count==2)
              start_loading <= 1;                                 
             end // read_count != write_count
          end // reset
     end    // always
endmodule
