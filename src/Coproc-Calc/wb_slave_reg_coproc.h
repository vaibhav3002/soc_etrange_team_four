/************************************************************
 *
 *      File : wb_slave_reg_coproc.h
 *      Author: A. Schindler
 *      Credits: T. Graba
 *      Telecom ParisTECH
 *
 *      Wishbone slave to buffer the coefficients between 
 *      LM32 and coprocessor
 *
 ************************************************************/


#ifndef __SLAVE_REG_COPROC_WB__
#define __SLAVE_REG_COPROC_WB__

#include <systemc>
#include "wb_slave_module.h"

namespace soclib { namespace caba {

    template<typename wb_param>
        class WbSlaveRegModuleCoproc : public WbSlaveModule<wb_param>
    {
	public:
	    WbSlaveRegModuleCoproc ( sc_module_name name ) : WbSlaveModule<wb_param>::WbSlaveModule(name) 
		{
		  /* initialises the written and read signals keeping track of whether data is new and has been read */
		    written = 0;
		    read = 0;
		}

      /* Registers for x coefficients */
	sc_signal<uint32_t> reg_p0_x; 
	sc_signal<uint32_t> reg_s0_x;
	sc_signal<uint32_t> reg_s1_x;
	sc_signal<uint32_t> reg_r0_x;
	sc_signal<uint32_t> reg_r1_x;
	sc_signal<uint32_t> reg_r2_x;
	sc_signal<uint32_t> reg_q0_x;
	sc_signal<uint32_t> reg_q1_x;
	sc_signal<uint32_t> reg_q2_x;
	sc_signal<uint32_t> reg_q3_x;
	sc_signal<uint32_t> reg_p0_y; 

      /* Registers for y coefficients */
	sc_signal<uint32_t> reg_s0_y;
	sc_signal<uint32_t> reg_s1_y;
	sc_signal<uint32_t> reg_r0_y;
	sc_signal<uint32_t> reg_r1_y;
	sc_signal<uint32_t> reg_r2_y;
	sc_signal<uint32_t> reg_q0_y;
	sc_signal<uint32_t> reg_q1_y;
	sc_signal<uint32_t> reg_q2_y;
	sc_signal<uint32_t> reg_q3_y;

	int written; // high high if new data is available (after 20 writes)
	int read; // read high if all 20 coeffs have been read

	sc_signal<bool> ready; // high if written = 20
	sc_signal<bool> loaded; // high of read = 20
	
	uint32_t slave_read(uint32_t ADDR) {
	    switch(ADDR)
	    {
	    case 0:
		return (reg_p0_x.read());
		break;
	    case 4:
		return (reg_s0_x.read());
		break;
	    case 8:
		return (reg_s1_x.read());
		break;
	    case 12:
		return (reg_r0_x.read());
		break;
	    case 16:
		return (reg_r1_x.read());
		break;
	    case 20:
		return (reg_r2_x.read());
		break;
	    case 24:
		return (reg_q0_x.read());
		break;
	    case 28:
		return (reg_q1_x.read());
		break;
	    case 32:
		return (reg_q2_x.read());
		break;
	    case 36:
		return (reg_q3_x.read());
		break;
	    case 40:
		return (reg_p0_y.read());
		break;
	    case 44:
		return (reg_s0_y.read());
		break;
	    case 48:
		return (reg_s1_y.read());
		break;
	    case 52:
		return (reg_r0_y.read());
		break;
	    case 56:
		return (reg_r1_y.read());
		break;
	    case 60:
		return (reg_r2_y.read());
		break;
	    case 64:
		return (reg_q0_y.read());
		break;
	    case 68:
		return (reg_q1_y.read());
		break;
	    case 72:
		return (reg_q2_y.read());
		break;
	    case 76:
		return (reg_q3_y.read());
		break;
	    }	
	    read++;
	    if (read==20)
	    {
		loaded = 1;
		read = 0;
	    }
	   return 0;    
	};
	
	uint32_t slave_write(uint32_t ADDR, uint32_t DATA) {
	    if(!written)
	    {
		ready = 0;
		loaded = 0;
	    }
	    switch(ADDR)
	    {
	    case 0:
		reg_p0_x = DATA;
		break;
	    case 4:
		reg_s0_x = DATA;
		break;
	    case 8:
		reg_s1_x = DATA;
		break;
	    case 12:
		reg_r0_x = DATA;
		break;
	    case 16:
		reg_r1_x = DATA;
		break;
	    case 20:
		reg_r2_x = DATA;
		break;
	    case 24:
		reg_q0_x = DATA;
		break;
	    case 28:
		reg_q1_x = DATA;
		break;
	    case 32:
		reg_q2_x = DATA;
		break;
	    case 36:
		reg_q3_x = DATA;
		break;
	    case 40:
		reg_p0_y = DATA;
		break;
	    case 44:
		reg_s0_y = DATA;
		break;
	    case 48:
		reg_s1_y = DATA;
		break;
	    case 52:
		reg_r0_y = DATA;
		break;
	    case 56:
		reg_r1_y = DATA;
		break;
	    case 60:
		reg_r2_y = DATA;
		break;
	    case 64:
		reg_q0_y = DATA;
		break;
	    case 68:
		reg_q1_y = DATA;
		break;
	    case 72:
		reg_q2_y = DATA;
		break;
	    case 76:
		reg_q3_y = DATA;
		break;
	    }
	    written++;
	    if (written==20)
	    {
		ready = 1;
		written = 0;
	    }
		return 0xDE;
	};

    };
}
}

#endif
