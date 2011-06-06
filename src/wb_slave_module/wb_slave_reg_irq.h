#ifndef __SLAVE_REG_IRQ_WB__
#define __SLAVE_REG_IRQ_WB__

#include <systemc>
#include "wb_slave_module.h"

namespace soclib { namespace caba {

    template<typename wb_param>
        class WbSlaveRegIrqModule : public WbSlaveModule<wb_param>
    {
	public:
	    WbSlaveRegIrqModule ( sc_module_name name ) : WbSlaveModule<wb_param>::WbSlaveModule(name) 
		{
			irq_out=false;
		}

	sc_signal<uint32_t> reg; //Internal register that must be accessible from the outside world

	sc_core::sc_out<bool> irq_out;


	void slave_raiseIrq() {
		this->irq_out=false;
	}

	protected:


	uint32_t slave_read(uint32_t ADDR) {
		return (reg.read());
	};
	
	uint32_t slave_write(uint32_t ADDR, uint32_t DATA) {
		//Acknowledge any pending IRQ
		irq_out=true;
		reg = DATA;
		return 0xDE;
	};


    };
}
}

#endif
