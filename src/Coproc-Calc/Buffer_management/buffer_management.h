/*
 * =============================================================================
 *
 *       Filename:  Buffer_management.h
 *
 *    Description:  Buffer Management - Type WB master
 *                  This module manages the coprocessor buffer.  
 * 
 *
 *         Author:  Ted
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */
#ifndef __BUFFER_MANAGEMENT__
#define __BUFFER_MANAGEMENT__

#include <systemc>
#include "wb_master_module_tuille.h"
#include "../common.h"
#include "wb_slave_reg_irq.h"

#define TILE_SIZE ((uint32_t) 64 )
#define COPRO_BUFFER_SIZE 2*TILE_SIZE
#define COPRO_BUFFER_BLOCK_SIZE TILE_SIZE 
#define COPRO_BUFFER_ROW_SIZE ((uint32_t) 4) 
#define COPRO_BUFFER_MODULO COPRO_BUFFER_SIZE/COPRO_BUFFER_BLOCK_SIZE

namespace soclib { namespace caba {

    template<typename wb_param>
        class Buffer_management: sc_core::sc_module
    {
        public:
            //buffer_management_inputs
	    bool Address_Register_Written;
            sc_core::sc_in<bool> p_clk_100mhz;
            sc_core::sc_in<bool> p_resetn;
            sc_core::sc_in<unsigned short> dx_in;
            sc_core::sc_in<unsigned short> dy_in;
            sc_core::sc_in<unsigned char> x;
            sc_core::sc_in<unsigned char> y;
            //buffer_management_outputs
	    sc_core::sc_out<unsigned char> pixel_out_0;
	    sc_core::sc_out<unsigned char> pixel_out_1;
	    sc_core::sc_out<unsigned char> pixel_out_2;
	    sc_core::sc_out<unsigned char> pixel_out_3;
	    sc_core::sc_out<bool>tile_ready;
//	    sc_core::sc_out<bool>go_incremental;
	    sc_core::sc_out<bool>pixel_out_valid;
	    sc_core::sc_out<unsigned char>dx_out;
	    sc_core::sc_out<unsigned char>dy_out;
            soclib::caba::WbMaster<wb_param> p_wb;
	    sc_core::sc_out<bool> irq_out;
	    WbSlaveRegIrqModule<wb_param> reg0;
            sc_core::sc_in<bool> tile_done_x;
            sc_core::sc_in<bool> tile_done_y;
            sc_core::sc_in<bool> pixel_in_valid_x;
            sc_core::sc_in<bool> pixel_in_valid_y;
        private:
            
          //  sc_signal<bool> start_loading;
            // send data to ram 
            void process_lookup_pixel_intensity();
            void process_load_buffer_from_ram();
            uint32_t data[COPRO_BUFFER_SIZE];
            bool load_block;
            bool start_reading;
	    bool internal_go;
	    bool internal_go_ack;
            uint32_t * buffer_pnt;
            uint32_t writes_count;
            uint32_t reads_count;
	    uint32_t pixels_created;
            unsigned char registered_dx;
            unsigned char registered_dy;
	    void     wait_cycles (uint32_t delay);
            WbMasterModuleTuille<wb_param> master1;
	    enum state {lookup_00,lookup_01,lookup_10,lookup_11}; 
	    bool load_in_progress;
        protected:
            SC_HAS_PROCESS(Buffer_management);

        public:
            // constructor
            Buffer_management ( sc_module_name name );
    };
}}

#endif //__BUFFER_MANAGEMENT__

