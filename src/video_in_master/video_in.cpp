/*
 * =============================================================================
 *
 *       Filename:  video_in.cpp
 *
 *    Description:  Video IN - Type WB master
 *                  This module takes the pixels from video generator and stores 
 * 					on to ram.
 *
 *         Author:  Theodoros Theodoropoulos
 *         			Vaibhav Singh
 *					Thibault Porteboeuf
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

namespace soclib { namespace caba {

	tmpl(/**/)::Video_in (sc_module_name name )
		:sc_core::sc_module(name), p_clk("p_clk"),p_resetn("p_resetn"), master0(p_clk_100mhz,p_wb), reg0("reg0"), irq_out("videoinirq")

	{
		reg0.irq_out(irq_out);
		// sc thread
		SC_THREAD(process_write_buffer);
		// no sensitivity list, waits have to be explicit
		SC_THREAD(process_load_buffer_to_ram);

	}

	// write buffer process
	tmpl(void)::process_write_buffer()
	{
		bool reset_done;
		// handle a reset cmd
		uint32_t iterations=0; //For simulation purposes only
		uint32_t index;        //word index 
		uint32_t index_8;      //byte index
		uint32_t mask_and=0xFFFFFFFF;
		for (;;) {
			wait(p_clk.posedge_event());                                                     //next clock

			if (!p_resetn) // reset
			{
				reset_done = true;
				index=0;
				index_8=0;
				write_count=0;
				reg0.Written = false;
				reset_config=true;
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

				//				if (!reg0.Written) {
				//					//Skip
				//					continue;
				//				}

				if (frame_valid){
					if (line_valid){

						data[index]=data[index]|(pixel_in<<8*(index_8 % 4));
						data[index]=data[index]&(mask_and ^((~pixel_in)<<8*(index_8 % 4)));     //Inserting a byte into the uint32_t buffer

						index_8++;
						if ((index_8 % 4 ==0) && (index_8!=0))                                  //augmenting uint32_t counter
							index++;

						if (((index % BLOCK_SIZE)==0) && (index!=0)&& (index_8 % 4==0) ){  //augmenting counter of written blocks
							write_count++;
							write_count=write_count % BLOCK_MODULO;
						}              
						index=  index==VIDEO_IN_WINDOW_SIZE? 0:index;                           //checking if we are at the end of the buffer
					}
				}
			}

			iterations++;
			//         std::cout<< read_count <<"read_count"<< write_count <<"write_count"<< std::endl;
			//        if (iterations==640*480)
			//             sc_stop();
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
			wait(p_clk.posedge_event());
		}
#ifdef SOCLIB_MODULE_DEBUG
		std::cout
			<< name()
			<< " wait end @" << sc_time_stamp()
			<< std::endl;
#endif
	}

	tmpl(void):: process_load_buffer_to_ram (){
		// handle a reset cmd
		bool reset_done = false;         
		uint32_t mem;
		uint32_t initial_image_position;
		uint8_t mask[VIDEO_IN_WINDOW_SIZE] ;
		uint8_t * mask_pnt;
		uint8_t buffer_count;
		bool init_done = false;
		for (;;) {

			// wait rising edge of clk
			wait(p_clk_100mhz.posedge_event());
			if (!p_resetn) // reset
			{
				master0.reset();
				reset_done = true;
				buffer_pnt=data;
				//            mem=0x41000000;  //starting from a fixed address since we have processor latency
				buffer_count=0;

				read_count=0;
				for (int i=0;i<(int) VIDEO_IN_WINDOW_SIZE;i++){  //initialising mask array
					mask[i]=0xFF;
				}
				mask_pnt=mask;                                   //initialising pointer to array
			} else {

				if ((reg0.Written)&&(reset_config)&&(!(frame_valid))) {
					init_done=true;
					reset_config=false;
					mem=this->reg0.reg.read();
					initial_image_position= mem;
					printf("VIDEOIN DESTINATION ADDRESS 0x%x\n",mem);
				} else if (reset_config) {
					continue;
				} 

				if (init_done) // clk event
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
					//Check whether an unacknoledged IRQ has been raised

					if (read_count!=write_count){ //we load a piece of the buffer to ram  only when available 
						master0.wb_write_blk(mem,mask_pnt,buffer_pnt,BLOCK_SIZE);     //converting byte length to word length       
						mem=mem+(4*BLOCK_SIZE);
						if ((mem>RAM_BASE+RAM_SIZE -4*(BLOCK_SIZE)) || (mem-initial_image_position)>=0x0004b000) //load_next image memory adress once image is loaded
						{                                                                                            
							//                 // mem=master0.wb_read_at(VIDEO_IN_REG);
							//                  mem=reg0.reg.read();
							//                  initial_image_position=mem;
							//Raise an IrQ
							this->reg0.slave_raiseIrq();

							reset_config=true;
							reg0.Written=false;

						}
						read_count++;                                                    //read counter is 0 when we ave written second half 
						read_count=read_count % BLOCK_MODULO;                                       //of the buffer 1 when we have writen first half
						buffer_count++;                                                  //Counts times we have loaded blocks to ram               
						mask_pnt=&mask[read_count*(BLOCK_SIZE)];
						buffer_pnt=&data[read_count*(BLOCK_SIZE)];                    //point accordingly to multiple of half

					}
				}
			}      //thread that sends buffer data to ram
		}

	}

}
}// namespace

