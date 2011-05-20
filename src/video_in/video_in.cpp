/*
 * =============================================================================
 *
 *       Filename:  video_in.cpp
 *
 *    Description:  Video In: 
 *                  This module takes the pixels and puts them on ram
 *
 *         Author:  Vaibhav Singh, vsingh@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */


#include <cstdio>

#include <fstream>
#include <sstream>
#include <iomanip>

#include "video_in.h"
#include "../segmentation.h"


#define tmpl(x) template<typename wb_param> x Video_in<wb_param>

#define WRITE_ON_RAM_VERIFY 0

namespace soclib { namespace caba {

    tmpl(/**/)::Video_in (sc_module_name iname  , unsigned int buf_size )
	:sc_core::sc_module(iname), p_clk("p_clk"),p_resetn("p_resetn"),
	master0(p_clk,p_wb), BUFFER_SIZE(buf_size), pixel_fifo (buf_size)
    {
	// pixel in process
	SC_THREAD(pixel_in_process);

	// ram_out process
	SC_THREAD(ram_out_process);

	// Pixel co-ordinates	
	pixel_c = 0;
	pixel_l = 0;

    }

    tmpl(void)::pixel_in_process()
    {
	// handle a reset cmd
	bool reset_done = false;

	while(true)
	{
	    if(!reset_n)
	    {
		pixel_c = 0;
		pixel_l = 0;
		reset_done = true;
#ifdef SOCLIB_MODULE_DEBUG
		std::cout
		    << name()
		    << " received a reset"
		    << std::endl;
#endif
	    }
	    else
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
		}else {
		    // image valide
		    if (line_valid && frame_valid)
		    {
#ifdef SOCLIB_MODULE_DEBUG
			cout << name() << " valid ..." <<  " lines : " << pixel_l << "col :" << pixel_c << endl;
#endif
			if (pixel_c < p_WIDTH && pixel_l < p_HEIGHT)
			{
			    pixel_fifo.write(pixel_in); 
			    pixel_c++;
			}
			else
			{
			    cout << name() << " WARNING: Too many pixels..!!!!!" 
				<< " lines : " << pixel_l << " col : " << pixel_c << endl;
			    exit(-1);
			}
		    }
		    else {
			// synchro horizontale
			if (frame_valid && !line_valid)
			{
			    if (pixel_c == p_WIDTH)
			    {
				pixel_c = 0;
				pixel_l++;
			    }
			    else if (pixel_c != 0)
			    {
				cout << name() << " Warning.........!!" 
				    << " lines : " << pixel_l << "col :" << pixel_c << endl;
			    }
			}
			// synchro verticale
			else if (!frame_valid)
			{
			    if((pixel_c == p_WIDTH) && (pixel_l == p_HEIGHT -1))
			    {
				//cout << name() << " Updating frame " << endl;
				pixel_c = 0;
				pixel_l = 0;
			    }
			}
		    }
		}
	    }
	    // wait rising edge of clk
	    wait(pixel_clk.posedge_event());
	} // infinite loop
    }

    tmpl(void)::ram_out_process()
    {
#if 1
	// handle a reset cmd
	bool reset_done = false;
	unsigned char buffer;
	uint32_t offset = 0;

	for (;;) {

	    if (!p_resetn) // reset
	    {
		master0.reset();
		reset_done = true;
	        offset = 0;
	        start_video_out = false;
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
		    wait(p_clk.posedge_event());
		    continue;
		}else{

#ifdef SOCLIB_MODULE_DEBUG
		    std::cout
			<< " Trying to read from FIFO "
			<< " !"
			<< std::endl;
#endif
		    if(pixel_fifo.num_available() >=  4)
		    {
			// 4 bytes are available
			uint32_t temp_write = 0;
			
			for(uint8_t count = 0; count < 4; count++)
			{
			    // blocking call to pixel fifo
			    buffer = pixel_fifo.read(); 
			    temp_write |= (uint32_t (buffer << 8*count));
			}
			// write to ram - 4 cycles
			master0.wb_write_at(RAM_BASE + offset,0xf,temp_write);
#if 1
			// Indicating vide out to start
			if(!start_video_out)
			start_video_out = true;
#endif
#if WRITE_ON_RAM_VERIFY
			cout << " Data Written [" << temp_write << "] at time " << sc_time_stamp() << endl;
			// Verify the write by reading it 
			uint32_t temp_read = master0.wb_read_at(RAM_BASE + offset);		
			if (temp_read != temp_write)		
			    cout << " Error - Incorrect write on RAM " << endl;
#endif

			// increment ram pointer
			offset += 4;
			temp_write = 0;

			// reset ram pointer if maximum size is reached
			if(offset == RAM_BASE + RAM_SIZE)
			    offset = 0;
		    }
			
		}
	    } 
	    // wait rising edge of clk
	    wait(p_clk.posedge_event());

	} // infinite loop
#endif
    }


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
	    wait(p_clk.posedge_event());
	}
#ifdef SOCLIB_MODULE_DEBUG
	std::cout
	    << name()
	    << " wait end @" << sc_time_stamp()
	    << std::endl;
#endif
    }

}}// namespace

