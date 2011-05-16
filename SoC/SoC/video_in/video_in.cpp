/*
 * =============================================================================
 *
 *       Filename:  video_in.cpp
 *
 *    Description:  video_in module
 *		    based on the generic wb master module
 *
 *         Author:  Thibault Porteboeuf, thibault.porteboeuf@telecom-paristech.fr
 *	   Credit:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#include <systemc>

#include "video_in.h"
#include <video_gen.h>
#include "segmentation.h"

#define tmpl(x) template<typename wb_param> x VideoInModule<wb_param>

namespace soclib { namespace caba {

	template<typename wb_param>
	 void VideoInModule<wb_param>::clk_pix_event() {
		if (frame_valid==0) {
			//ACTIVATE WRITE SEQUENCE
			write=true;
			end_frame=true;
		}	
		else if (line_valid) {
			Buffer[pos] = pixel_in.read();
			pos++;
			pos=pos%BUFFER_SIZE;
			long delta = pos-offset;
			if (delta<0) {delta+=BUFFER_SIZE;}

			if (2*delta>=BUFFER_SIZE) {
				//ACTIVATE WRITE SEQUENCE
				write=true;
			}
		}

	}

	template<typename wb_param> 
	void VideoInModule<wb_param>::clk_wb_event() {
		if (write) {
			write=false;
			//CREATE A LOCAL COPY OF THE DATA TO BE TRANSFERED
			unsigned char local_buffer[BUFFER_SIZE];
			long p = offset;
			long idx = 0;
			long delta = p - pos;
			if (delta<0) {delta+=BUFFER_SIZE;}
			while (delta<4) {
				for (int i=0; i<4; i++) {
					local_buffer[idx] = Buffer[p];
					idx++;
					p++;
					p=p%BUFFER_SIZE;
				}
				delta = p - pos;
				if (delta<0) {delta+=BUFFER_SIZE;}
			}
			//FREEING SOME SPACE INSIDE THE SHARED BUFFER
			offset+=idx;
			offset=offset%BUFFER_SIZE;
			
			wb_write_blk(RAM_BASE,(uint32_t*) local_buffer,idx/4);
		}
	}

    ////////////////////////////////////////////////////////////////////////////
    // single read/write operation
    ////////////////////////////////////////////////////////////////////////////

    template<typename wb_param>
        uint32_t VideoInModule<wb_param>::wb_read_at
        ( uint32_t addr )
        {
            sc_core::wait(p_clk.negedge_event());
            p_wb.ADR_O = addr;
            p_wb.SEL_O = 0xF;
            p_wb.STB_O = true;
            p_wb.CYC_O = true;
            p_wb.WE_O  = false;

            // sc_core::wait for ack
            WaitWbAck();
            // clean request after clk falling edge
            sc_core::wait(p_clk.negedge_event());
            CleanWb();
            num_reads++;
            return (uint32_t) p_wb.DAT_I.read();
        }

    template<typename wb_param>
        void     VideoInModule<wb_param>::wb_write_at
        (uint32_t addr, uint8_t mask, uint32_t data)
        {
            // set request on clk falling edge
            sc_core::wait(p_clk.negedge_event());
            p_wb.DAT_O = data;
            p_wb.ADR_O = addr;
            p_wb.SEL_O = mask;
            p_wb.STB_O = true;
            p_wb.CYC_O = true;
            p_wb.WE_O  = true;

            // sc_core::wait for ack
            WaitWbAck();
            // clean request after clk falling edge
            sc_core::wait(p_clk.negedge_event());
            CleanWb();
            num_writes++;
        }

    ////////////////////////////////////////////////////////////////////////////
    // burst read/write operation to/from an uint32_t array
    ////////////////////////////////////////////////////////////////////////////

    template<typename wb_param>
        void VideoInModule<wb_param>::wb_read_blk
        ( uint32_t saddr, uint32_t num, uint32_t *dest)
        {
            for (uint32_t i = 0; i< num; i++)
            {
                sc_core::wait(p_clk.negedge_event());
                p_wb.ADR_O = saddr;
                p_wb.SEL_O = 0xF;
                p_wb.STB_O = true;
                p_wb.CYC_O = true;
                p_wb.WE_O  = false;

                // sc_core::wait for ack
                WaitWbAck();
                num_reads++;
                *dest++ = (uint32_t) p_wb.DAT_I.read();
                // next data
                saddr = saddr + 4;
            }
            // clean request after clk falling edge
            sc_core::wait(p_clk.negedge_event());
            CleanWb();
        }

    template<typename wb_param>
        void     VideoInModule<wb_param>::wb_write_blk
        ( uint32_t saddr, uint32_t *data, uint32_t num)
        {
            for (uint32_t i = 0; i< num; i++)
            {
                // set request on clk falling edge
                sc_core::wait(p_clk.negedge_event());
                p_wb.DAT_O = *data++;
                p_wb.ADR_O = saddr;
                p_wb.SEL_O = 0xFF;
                p_wb.STB_O = true;
                p_wb.CYC_O = true;
                p_wb.WE_O  = true;

                // sc_core::wait for ack
                WaitWbAck();
                num_writes++;
                // next data
                saddr = saddr + 4;
            }
            // clean request after clk falling edge
            sc_core::wait(p_clk.negedge_event());
            CleanWb();
        }

    // Reset
    template<typename wb_param>
        void     VideoInModule<wb_param>:: reset()
        {
            trans_wait_cycles    = 0;
            num_writes           = 0;
            num_reads            = 0;
            CleanWb();
        }

    // print simulation statistics
    template<typename wb_param>
        void     VideoInModule<wb_param>:: print_stats()
        {
            std::cout << *this << std::endl;
        }

    ////////////////////////////////////////////////////////////////////////////
    //  Constructor
    ////////////////////////////////////////////////////////////////////////////

    template<typename wb_param>
        VideoInModule<wb_param>::VideoInModule ( 
                sc_core::sc_in<bool> &p_clk,
                soclib::caba::WbMaster<wb_param> &p_wb
                ): p_clk(p_clk),p_wb(p_wb)
        {
		this->p_clk	(p_clk);
		this->p_wb	(p_wb);

		SC_THREAD(clk_pix_event);
		sensitive << clk_pix.pos();
		dont_initialize();

		SC_THREAD(clk_wb_event);
		sensitive << p_clk;
		dont_initialize();
        }
}}

