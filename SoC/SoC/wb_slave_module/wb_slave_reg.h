#ifndef __SLAVE_REG_WB__
#define __SLAVE_REG_WB__

#include <systemc>
#include "wb_slave_module.h"

namespace soclib { namespace caba {

    template<typename wb_param>
        class WbSlaveRegModule : public WbSlaveModule<wb_param>
    {
	public:
	    WbSlaveRegModule ( sc_core::sc_in<bool> &p_clk, sc_core::sc_in<bool> &p_resetn, soclib::caba::WbSlave<wb_param> &p_wb) : WbSlaveModule(p_clk, p_resetn, p_wb) 
		{
		}
	protected:

	uint32_t reg; //Internal register

	uint32_t slave_read(uint32_t ADDR) {
		return (reg);
	};
	
	uint32_t slave_write(uint32_t ADDR, uint32_t DATA) {
		reg = DATA;
		return 0x0;
	};

    };
}}

#endif
