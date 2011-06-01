#ifndef __SLAVE_REG_WB__
#define __SLAVE_REG_WB__

#include <systemc>
#include "wb_slave_module.h"

namespace soclib { namespace caba {

    template<typename wb_param>
        class WbSlaveRegModule : public WbSlaveModule<wb_param>
    {
	public:
	    WbSlaveRegModule ( sc_module_name name ) : WbSlaveModule<wb_param>::WbSlaveModule(name) 
		{
		}

	sc_signal<uint32_t> reg; //Internal register that must be accessible from the outside world

	protected:


	uint32_t slave_read(uint32_t ADDR) {
		return (reg.read());
	};
	
	uint32_t slave_write(uint32_t ADDR, uint32_t DATA) {
		reg = DATA;
		return 0xDE;
	};

    };
}}

#endif
