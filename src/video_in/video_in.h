/*
 * =============================================================================
 *
 *       Filename:  video_in.h
 *
 *    Description:  Video In Module
 *                  This module takes the pixels from the video generator and stores them on ram
 *
 *         Author:  Vaibhav Singh, vsingh@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef __VIDEO_IN__
#define __VIDEO_IN__


#include <systemc>

#include "wb_master_module.h"

#define p_WIDTH      640 
#define p_HEIGHT     480
#define p_LINE_SYNC  160
#define p_FRAME_SYNC 40

using namespace std;

namespace soclib { namespace caba {

    template<typename wb_param>
        class Video_in: sc_core::sc_module
    {
        public:
            sc_core::sc_in<bool> p_clk;
            sc_core::sc_in<bool> p_resetn;
            soclib::caba::WbMaster<wb_param> p_wb;

// For the moment Video-In will indicate video-out to start
            sc_core::sc_out<bool>        start_video_out;

	// module interfacing video generator	    
	    sc_in_clk           pixel_clk;
	    sc_in<bool>         reset_n;

	    sc_in<bool>        line_valid;
	    sc_in<bool>        frame_valid;

	    sc_in<unsigned char> pixel_in;

        private:
	    // ram_out: reads pixels from caches and writes on cache
	    void ram_out_process();
            // pixel_in  thread: Takes pixels and stores on to cache
            void pixel_in_process();
            
            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master0;

            // Image Cache 
	    const  unsigned int BUFFER_SIZE;
	    sc_fifo<unsigned char> pixel_fifo;

	    unsigned int pixel_c; 
	    unsigned int pixel_l; 


        protected:
            SC_HAS_PROCESS(Video_in);

        public:
            // constructor
            Video_in ( sc_module_name name , unsigned int buf_size = 256);
    };
}}

#endif //__VIDEO_IN__
