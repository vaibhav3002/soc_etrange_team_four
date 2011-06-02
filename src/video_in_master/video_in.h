/*
 * =============================================================================
 *
 *       Filename:  master.h
 *
 *    Description:  generic wb master
 *                  This module demonstrate WbMasterModule usage
 *
 *         Author:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef __VIDEO_IN_H__
#define __VIDEO_IN_H__

#include <systemc>
#include "wb_master_module.h"
#include "../video_buffer.h"
#include "wb_slave_reg.h"

#define VIDEO_IN_WINDOW_SIZE  VIDEO_BUFFER_SIZE 
#define BLOCK_SIZE VIDEO_BUFFER_BLOCK_SIZE //this defines the size of the
                                                        //the block sent to ram   
#define BLOCK_MODULO (VIDEO_IN_WINDOW_SIZE/BLOCK_SIZE)


namespace soclib { namespace caba {

    template<typename wb_param>
        class Video_in: sc_core::sc_module
    {
        public:
            
            sc_core::sc_in<bool> p_clk;
            
            sc_core::sc_in<bool> p_clk_100mhz;
            sc_core::sc_in<bool> p_resetn;
            sc_core::sc_in<bool> line_valid;
            sc_core::sc_in<bool> frame_valid;
            sc_core::sc_in<unsigned char> pixel_in;
            soclib::caba::WbMaster<wb_param> p_wb;
            sc_core::sc_out<bool> start_loading;
            WbSlaveRegModule<wb_param> reg0;
        private:
            
            // send data to ram 
            void process_write_buffer();
            void process_load_buffer_to_ram();
            uint32_t data[VIDEO_IN_WINDOW_SIZE];
            int read_count;
            int write_count;
            uint32_t * buffer_pnt;
            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master0; 
        protected:
            SC_HAS_PROCESS(Video_in);

        public:
            // constructor
            Video_in ( sc_module_name name );
    };
}}

#endif //__MASTER__


