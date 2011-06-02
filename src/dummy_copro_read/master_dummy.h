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

#ifndef __MASTER_DUMMY__
#define __MASTER_DUMMY__

#include <systemc>
#include "wb_master_module.h"
#include "../common.h"
//#define COPRO_IN_WINDOW_SIZE  VIDEO_BUFFER_SIZE 
#define COPRO_IN_BLOCK_SIZE VIDEO_BLOCK_RD_WR_SIZE   


namespace soclib { namespace caba {

    template<typename wb_param>
        class Master_dummy: sc_core::sc_module
    {
        public:
            
//             sc_core::sc_in<bool> p_clk;
            sc_core::sc_in<bool> p_clk_100mhz;
            sc_core::sc_in<bool> p_resetn;
            soclib::caba::WbMaster<wb_param> p_wb;
             
        private:
            
            // send data to ram 
            void process_read_from_ram();
            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master0;

            
        protected:
            SC_HAS_PROCESS(Master_dummy);

        public:
            // constructor
            Master_dummy ( sc_module_name name );
    };
}}

#endif //__MASTER__


