/*
 * =============================================================================
 *
 *       Filename:  video_out.h
 *
 *    Description:  Video Out Module
 *                  This module takes the pixels from the ram and give them to display module
 *
 *         Author:  Vaibhav Singh, vsingh@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef __VIDEO_OUT__
#define __VIDEO_OUT__


#include <systemc>

#include "wb_master_module.h"

#define p_WIDTH      640 
#define p_HEIGHT     480
#define p_LINE_SYNC  160
#define p_FRAME_SYNC 40

using namespace std;

namespace soclib { namespace caba {

    template<typename wb_param>
        class Video_out: sc_core::sc_module
    {
        public:
            sc_core::sc_in<bool> p_clk;
            sc_core::sc_in<bool> p_resetn;
            soclib::caba::WbMaster<wb_param> p_wb;
            sc_core::sc_in<bool> start_video_out;

	// module interfacing video generator	    
	    sc_in_clk           pixel_clk;
	    sc_in<bool>         reset_n;

	    sc_out<bool>        line_valid;
	    sc_out<bool>        frame_valid;

	    sc_out<unsigned char> pixel_out;

        private:
	    // ram_in: reads pixels from ram and writes on fifo
	    void ram_in_process();
            // pixel_out  thread: Reads from fifo and sends to display
            void pixel_out_process();
            
            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master0;

            // Image Cache 
	    const  unsigned int BUFFER_SIZE;
	    sc_fifo<unsigned char> pixel_fifo;

	    unsigned int pixel_c; 
	    unsigned int pixel_l; 


        protected:
            SC_HAS_PROCESS(Video_out);

        public:
            // constructor
            Video_out ( sc_module_name name , unsigned int buf_size = 8);
    };
}}

#endif //__VIDEO_OUT__
