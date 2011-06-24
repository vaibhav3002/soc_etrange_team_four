/*
 * =============================================================================
 *
 *       Filename:  buffer_management.cpp
 *
 *    Description:  Buffer management - Type WB master/Slave
 *                  This module takes the tiles stored on the ram and stores them  
 * 		    on the local coprocessor buffer. It also performs lookup of 
 *		    the addresses given from the incremental module
 *         Author:  Ted
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */
#include <cstdio>

#include <fstream>
#include <sstream>
#include <iomanip>

#include "../segmentation.h"

#define tmpl(x) template<typename wb_param> x Buffer_management <wb_param>

namespace soclib { namespace caba {

   tmpl(/**/)::Buffer_management (sc_module_name name )
      :sc_core::sc_module(name),p_resetn("p_resetn"), master1(p_clk_100mhz,p_wb), reg0("coproc_out_slave_reg") 

   {
      reg0.irq_out(irq_out);
      // sc thread
      SC_THREAD(process_load_buffer_from_ram);
      // no sensitivity list, waits have to be explicit
      SC_THREAD(process_lookup_pixel_intensity);
   }

   // write buffer process
   tmpl(void)::process_load_buffer_from_ram()
   {
      bool reset_config = false; 
      bool reset_done;
      // handle a reset cmd
      uint32_t index_8;
      uint32_t index;
      uint32_t * buffer_pnt;
      uint32_t mem;
      uint32_t initial_image_position;
      bool initial_write;
      for (;;) {

         wait(p_clk_100mhz.posedge_event());


	 if (!p_resetn) // reset
         {
//	    reg0.slave_raiseIrq();
            reset_done = true;
            index=0;
            master1.reset();
            index_8=0;
            data[0]=0;
            buffer_pnt=data;
            start_reading=false;
            initial_write=true;
	    reset_config = true;
            writes_count=0;
	    Address_Register_Written=true;//to be removed after testing
         }  
         else // clk event
         {
            // check if the reset has been done
            if (!reset_done)
            {
#ifdef SOCLIB_MODULE_DEBUG
               std::cout
                  << name()
                  << " : has not been reseted yet!!!"
                  << std::endl;
#endif
               continue;
            }
	    if (reset_config) {
		    if (reg0.Written==true)
		{   //read_ mem address
		    mem=reg0.reg.read();  
		    initial_image_position=mem;
		    reset_config=false;
		    //initial_write=true;
//		    printf("COPRO_BUFFER IS READING FROM MEM ADDRESS 0x%x\n",mem);
		}
	    } else {

               //std::cout<<writes_count<<reads_count<<std::endl;
               if ((writes_count!=reads_count) || initial_write ){           //first time we will load data immediately. After that we will write only once                                                                                                 //a vlock of data has been sent to the output
               master1.wb_read_blk ( mem , COPRO_BUFFER_BLOCK_SIZE , buffer_pnt); 
        //       std::cout<<"wrote a block"<<std::endl;
               writes_count++;                                              //Once we have loaded data from ram we increase counter
               writes_count=writes_count % COPRO_BUFFER_MODULO;
              // printf("COPRO_BUFFER WROTE TILE N %d\n",writes_count); 
               buffer_pnt=&data[writes_count*COPRO_BUFFER_BLOCK_SIZE];         //next buffer block address
               if(writes_count==1){
               start_reading=true;
	       tile_ready=true; //notify the incremental  that we have a ready tile to the lookup process
               }
               initial_write=false;                                        
               reset_config=true; //After each block has been written we have to wait for the new address
		}
            }
         }
      }

   } // infinite loop
   
    // wait cycles
    tmpl(void):: wait_cycles (uint32_t delay)
    {
#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << name()
            << " wait start @" << sc_time_stamp()
            << std::endl;
#endif
        while (delay--) {
            wait(p_clk_100mhz.posedge_event());
        }
#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << name()
            << " wait end @" << sc_time_stamp()
            << std::endl;
#endif
    }

    tmpl(void):: process_lookup_pixel_intensity (){
      // handle a reset cmd
      bool reset_true=0;
      uint32_t index;
      uint32_t registered_index;
      uint32_t index_8;
      uint32_t registered_index_8;
      state lookupstate;
      for (;;) {

	if (!p_resetn) // reset
	{  
	  load_block=true;
	  index=0;
	  index_8=0;
	  reads_count=0;
	  pixel_out_valid=false;
	  lookupstate=lookup_00;
	  //  reset_done = true;
	  load_in_progress = false;

	}
	else // clk event
	{

	  // std::cout<<"video out buffer out running<<std"<<std::endl;             
	  //write application here
	  if (start_reading)
	 {
               if ((writes_count!=reads_count) ||load_in_progress ){
		load_in_progress = true;
		if(pixel_in_valid)
		{
			    pixel_out_valid=true;
			if(x + 16*y < 256)		
			{
			    registered_index=((x+16*y)-((x+16*y)%4))/4;
			    registered_index_8=(x+16*y) % 4;

		// For the pixel_0
			    index=((x+16*y)-((x+16*y)%4))/4;
			    index_8=(x+16*y) % 4;
			   //getting 8 bits value out of an 32 bits register 
			    pixel_out_0 = (uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8)  ;        
			    dx_out = ((0xFF00 & dx_in)>>8) ;
			    dy_out = ((0xFF00 & dy_in)>>8) ;
	
		// For the pixel_1

	
			    index += COPRO_BUFFER_ROW_SIZE;//point to pixel_01
			    if(index<64)
			    {
				pixel_out_1 = (uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8);
			    }else 
			    {
				pixel_out_1 =128;
			    }

			    // For the pixel_2

		  if  ((registered_index_8==3) && (registered_index % 4 ==3 ))
		{
			 pixel_out_2 = 128;
		}else
		{
		 	if (registered_index_8==3)
			{
			    index=registered_index+1;
			    index_8=0;

			}else
			{
			    index=registered_index;
			    index_8=registered_index_8+1;

			}	

			pixel_out_2=(uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8);
		}
			
			// For the pixel 3 
			if(registered_index_8==3)
			
			{
			
			index=registered_index+COPRO_BUFFER_ROW_SIZE +1;
			index_8=0;	
			
			}else 
			{
			
			index=registered_index+COPRO_BUFFER_ROW_SIZE;
			index_8=registered_index_8+1;

			}

			if (index<64)
			{

			    pixel_out_3 = (uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8);

			}else 
			{

			    pixel_out_3 = 128;

			}


			} else {
				dx_out = 0;
				dy_out = 0;
				pixel_out_0 = 0;
				pixel_out_1 = 0;
				pixel_out_2 = 0;
				pixel_out_3 = 0;
			}
		pixels_created++;
			

		}else
		{
		    pixel_out_valid=false;	

		}

#if 0
	    switch (lookupstate)
	    {
	      case lookup_00:
		if (pixel_in_valid)
		{
		  if ((x+16*y)<256)
		  {		
		    registered_index=((x+16*y)-((x+16*y)%4))/4;
		    registered_index_8=(x+16*y) % 4;
		    index=registered_index;
		    index_8=registered_index_8;
//		    printf("LOOKED UP DATA AT TILE WORD POSITION N %d\n  AND BYTE POSITION %d\n ",index,index_8);
		    pixel_out= (uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8)  ;        //getting 8 bits value out of an 32 bits register 
		    registered_dx=((0xFF00 & dx_in)>>8) ;
		    registered_dy=((0xFF00 & dy_in)>>8) ;
		    dx_out=registered_dx;
		    dy_out=registered_dy;
		    pixel_out_valid=true;
		    if ((dx_in==0)&&(dy_in==0))
		    {
		      lookupstate=lookup_00;//no need to interpolate wait for the next pixel		
		      pixels_created++;			
		    }else 
		    {
		      lookupstate=lookup_01;
		    }
		  }else 
		  {
		    std::cout<<"Requested Pixel_00 is out of buffer range"<<std::endl;
		    pixel_out=255; //in case we are out of buffer range
		    dx_out=0;  
		    dy_out=0;  
		    pixel_out_valid=true;
		    lookupstate=lookup_01;
		    pixels_created++;  
		  }  

		}
		else
		  pixel_out_valid=false;	
		break;
	      case lookup_01:
		index=registered_index+COPRO_BUFFER_ROW_SIZE;//point to pixel_01
		index_8=registered_index_8;
		if(index<64)
		{
		  pixel_out=(uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8);
		  pixel_out_valid=true;
		  dx_out=registered_dx;
		  dy_out=registered_dy;
		  lookupstate=lookup_10;
		}else 
		{
		  std::cout<<"Requested Pixel_01 is out of buffer range"<<std::endl; 
		  pixel_out=128;
		  pixel_out_valid=true;
                  dx_out=registered_dx;                                                         
                  dy_out=registered_dy;    
		  lookupstate=lookup_10;
		}
		break;
	       case lookup_10:
		  if  ((registered_index_8==3) && (registered_index % 4 ==3 ))
		{
			 std::cout<<"Requested Pixel_10 is out of buffer range"<<std::endl;
			 pixel_out=128;
		  	 pixel_out_valid=true;
			 dx_out=registered_dx;
			 dy_out=registered_dy;
			 lookupstate=lookup_11;
		}else
		{
		 	if (registered_index_8==3)
			{
				index=registered_index+1;
		        	index_8=0;
			
			}else
			{
				index=registered_index;
				index_8=registered_index_8+1;
		
			}	
		    
		   	pixel_out=(uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8);
		  	pixel_out_valid=true;
		   	dx_out=registered_dx;
                   	dx_out=registered_dy;
		   	lookupstate=lookup_11;
		}
			break;		
		case lookup_11:
			
			if(registered_index_8==3)
			
			{
			
			index=registered_index+COPRO_BUFFER_ROW_SIZE +1;
			index_8=0;	
			
			}else 
			{
			
			index=registered_index+COPRO_BUFFER_ROW_SIZE;
			index_8=registered_index_8+1;
			
			}
		
			if (index<64)
			{
			
			pixel_out=(uint8_t) ((data[reads_count*COPRO_BUFFER_BLOCK_SIZE+index] & ( 0xFF<<8*index_8 ))>>8*index_8);
                        pixel_out_valid=true;
                        dx_out=registered_dx;
                        dx_out=registered_dy;
                        lookupstate=lookup_00;

			}else 
			{
			
			std::cout<<"Requested Pixel_11 is out of buffer range"<<std::endl;
			pixel_out=128;
                        pixel_out_valid=true;
                        dx_out=registered_dx;
                        dx_out=registered_dy;
                        lookupstate=lookup_00;

			}
		break;
		default: 
			;//debug purposes
	    }
#endif

//	  go_incremental = true;

} else 
	{
//	  go_incremental = false;
	}

}else {
	  pixel_out_valid=false;
	}	  
	  if (pixels_created==4*TILE_SIZE)
	{
	load_in_progress = false;
	pixels_created=0;
	reads_count++;
	reads_count=reads_count % COPRO_BUFFER_MODULO;	
	this->reg0.slave_raiseIrq();
       // printf("COPRO_BUFFER LOOKED UP TILE N %d\n",reads_count); 
	}

	}

      wait(p_clk_100mhz.posedge_event());
      }
      // wait rising edge of clk
    }      //thread that sends buffer data to ram




}
}// namespace

