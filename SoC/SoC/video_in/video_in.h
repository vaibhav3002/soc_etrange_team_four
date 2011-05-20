/*
 * =============================================================================
 *
 *       Filename:  video_in.h
 *
 *    Description:  video in master
 *
 *         Author:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef __VIDEO_IN__
#define __VIDEO_IN__


#include <systemc>

#include "wb_slave_reg.h"
#include "wb_master_module.h"

#define BUFFER_SIZE 512

namespace soclib { namespace caba {

    template<typename wb_param>
        class VideoInModule: sc_core::sc_module
    {
        public:
            sc_core::sc_in<bool> p_clk;
            sc_core::sc_in<bool> p_resetn;
            soclib::caba::WbMaster<wb_param> p_wb;

	    sc_core::sc_in<bool> p_clk_pix;
	    sc_core::sc_in<bool> reset_n;
	    sc_core::sc_in<bool> line_valid;
	    sc_core::sc_in<bool> frame_valid;
	    sc_core::sc_in<unsigned char> pixel_in;

        private:
            // main thread
            void process();
            sc_fifo<unsigned char>* pix_fifo;
	    long offset;
	    bool write;
	    bool end_frame;

            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master0;

	    WbSlaveRegModule<wb_param> slave;

	    void clk_pix_event();
	    void clk_wb_event();

        protected:
            SC_HAS_PROCESS(VideoInModule);

        public:
            // constructor
            VideoInModule ( sc_module_name name );
    };
}}

#endif //__MASTER__
