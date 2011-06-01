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

#ifndef __MASTER_DUMMY_WRITE__
#define __MASTER_DUMMY_WRITE__


#include <systemc>
#include "wb_master_module.h"
#include "../common.h"
//#define COPRO_OUT_WINDOW_SIZE  VIDEO_BUFFER_SIZE 
#define COPRO_OUT_BLOCK_SIZE  VIDEO_BLOCK_RD_WR_SIZE


namespace soclib { namespace caba {

    template<typename wb_param>
        class Master_dummy_write: sc_core::sc_module
    {
        public:
            
             sc_core::sc_in<bool> p_clk;
            sc_core::sc_in<bool> p_clk_100mhz;
            sc_core::sc_in<bool> p_resetn;
            soclib::caba::WbMaster<wb_param> p_wb;
             
        private:
            
            // send data to ram 
            void process_write_to_ram();
            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master1;

            
        protected:
            SC_HAS_PROCESS(Master_dummy_write);

        public:
            // constructor
            Master_dummy_write ( sc_module_name name );
    };
}}

#endif //__MASTER__


