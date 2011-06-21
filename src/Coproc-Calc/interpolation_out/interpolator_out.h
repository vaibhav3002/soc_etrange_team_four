/*:v
 * =============================================================================
 *
 *       Filename:  interpolator_out.h
 *
 *    Description:  Interpolator Out - Type WB master/slave
 *                  This module takes the pixels from the interpolator and stores 
 * 					on to ram.
 *
 *         Author:  Ted
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef __INTERPOLATOR_OUT_H__
#define __INTERPOLATOR_OUT_H__

#include <systemc>
#include "wb_master_module_tuille.h"
#include "../common.h"
#include "wb_slave_reg_irq.h"

#define INTERPOLATOR_OUT_WINDOW_SIZE ((uint32_t)128) //buffer size is two times tile size 
#define INTERPOLATOR_OUT_BLOCK_SIZE ((uint32_t) 64) //this is the size of a tile
                                                           
#define INTERPOLATOR_OUT_BLOCK_MODULO (INTERPOLATOR_OUT_WINDOW_SIZE/INTERPOLATOR_OUT_BLOCK_SIZE)


namespace soclib { namespace caba {

    template<typename wb_param>
        class Interpolator_out: sc_core::sc_module
    {
        public:
            
            sc_core::sc_in<bool> p_clk_100mhz;
            sc_core::sc_in<bool> p_resetn;
            sc_core::sc_in<bool> pixel_valid;
            sc_core::sc_in<unsigned char> pixel_in;
	    sc_core::sc_out<bool> irq_out;
            soclib::caba::WbMaster<wb_param> p_wb;
            WbSlaveRegIrqModule<wb_param> reg0;

        private:            
            // send data to ram 
            void process_write_buffer();
            void process_load_buffer_to_ram();
            uint32_t data[INTERPOLATOR_OUT_WINDOW_SIZE];
            int read_count;
            int write_count;
            uint32_t * buffer_pnt;
            void     wait_cycles (uint32_t delay);
            WbMasterModuleTuille<wb_param> master0;
	    bool frame_valid_mem;
	    bool reset_config; 

        protected:
            SC_HAS_PROCESS(Interpolator_out);

        public:
            // constructor
            Interpolator_out ( sc_module_name name );
    };
}}

#endif //__INTERPOLATOR_OUT__


