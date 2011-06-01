/*
 * =============================================================================
 *
 *       Filename:  video_out.cpp
 *
 *    Description:  Video Out: 
 *                  This module takes the pixels from the ram and gives to
 display module
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

#include "video_out.h"
#include "../segmentation.h"


#define tmpl(x) template<typename wb_param> x Video_out<wb_param>



namespace soclib { namespace caba {

	tmpl(/**/)::Video_out (sc_module_name iname  , unsigned int buf_size )
		:sc_core::sc_module(iname), p_clk("p_clk"),p_resetn("p_resetn"),
		master0(p_clk,p_wb), BUFFER_SIZE(buf_size), pixel_fifo (buf_size)
	{
		// pixel out process
		SC_THREAD(pixel_out_process);

		// ram_in process
		SC_THREAD(ram_in_process);

	}

	tmpl(void)::pixel_out_process()
	{
#if 1
		// handle a reset cmd
		bool reset_done = false;

		while(true)
		{
			if(!reset_n)
			{
				reset_done = true;
				// resetting all outputs
				pixel_out = 0;
				line_valid = false;
				frame_valid = false;

#ifdef SOCLIB_MODULE_DEBUG
				std::cout
					<< name()
					<< " received a reset"
					<< std::endl;
#endif
				wait(start_video_out.posedge_event());

				// Wait for 1 cycles after the indication to start to allow ram in process to write something into buffer
				wait(pixel_clk.posedge_event());
				wait(pixel_clk.posedge_event());
				wait(pixel_clk.posedge_event());
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
					// wait rising edge of clk
					wait(pixel_clk.posedge_event());
					continue;
				}else {
					unsigned int i,j;
					for(i=0; i<( p_HEIGHT + p_FRAME_SYNC ); i++)
						for(j=0; j<( p_WIDTH + p_LINE_SYNC ); j++)
						{
							// l'image active est de p_WIDTH*p_HEIGHT
#ifdef SOCLIB_MODULE_DEBUG
							//			    cout << " Pixel Out i = [" << i << "] j = [" << j << "] " << endl;
#endif
							if((i<p_HEIGHT) && (j>p_LINE_SYNC-1))
							{
#if BLOCKING_READ
								pixel_out = pixel_fifo.read();
#else
								uint8_t temp_pixel;
								if(!pixel_fifo.nb_read(temp_pixel))
									cout << " WARNING: Synchronisation is not proper between RAM IN and PIXEL OUT " << endl;
								else{
									pixel_out = temp_pixel;
									// std::cout << name() << " Pixel out is [" << pixel_out << " for i = [" << i << "] and j = [" << j << "]" << endl;
								}
#endif
							}
							else
								pixel_out = 0xBB;

							// Generation de line valid
							line_valid = (i<p_HEIGHT) && (j>p_LINE_SYNC-1);

							// Generation de frame valid
							frame_valid = (i<p_HEIGHT);

							// wait rising edge of clk
							wait(pixel_clk.posedge_event());

						}
				} 
			}
		} // infinite loop
#endif
	}


	tmpl(void)::ram_in_process()
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
				/*	
					Instead of waiting for certain interval, wait for start signal
				// Wait for certain clock cycles to make sure that video_in has put pixel data on ram.
				wait_cycles(800);
				continue;
				 */


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

				}else{

					if(!start_video_out)
					{
						cout << " RAM In Process Waiting for start signal " << endl; 
						wait(start_video_out.posedge_event());
						cout << " RAM in Process - receieved start signal " << endl;
					}

					// 4 bytes are available
					uint32_t temp_read = master0.wb_read_at(RAM_BASE + offset);		
#ifdef SOCLIB_MODULE_DEBUG
					cout << " Ram In read value = [" << temp_read << "]  at address " << RAM_BASE + offset << endl;
#endif			

					// put into fifo 
					for(uint8_t count = 0; count < 4; count++)
					{
						// blocking call to pixel fifo
						buffer = (temp_read >> 8*count) & 0xFF;
#ifdef SOCLIB_MODULE_DEBUG
						cout << " Ram In FIFO buffer value = [" << buffer << "] " << endl;
#endif			
						pixel_fifo.write(buffer);
					}
					// increment ram pointer
					offset += 4;

					// reset ram pointer if maximum size is reached
					if(offset == RAM_BASE + RAM_SIZE)
						offset = 0;
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

