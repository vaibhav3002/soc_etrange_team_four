/*
 * =============================================================================
 *
 *       Filename:  wb_slave_reg_irq.h
 *
 *    Description:  one register wishbone slave including irq wire
 *
 *	   Author:  Thibault Porteboeuf
 *     Company:  Telecom Paris Tech
 *
 * =============================================================================
 */




#ifndef __SLAVE_REG_IRQ_WB__
#define __SLAVE_REG_IRQ_WB__

#include <systemc>
#include "wb_slave_module.h"
#include "wb_slave.h"

namespace soclib { namespace caba {


    template<typename wb_param>
        class WbSlaveRegIrqModule : public WbSlaveModule<wb_param>
    {

	public:


	    WbSlaveRegIrqModule ( sc_module_name name ) : WbSlaveModule<wb_param>::WbSlaveModule(name), irq_out("irq_out") 
		{
			Written=false;
	//		irq_out=false;
		}

	sc_signal<uint32_t> reg; //Internal register that must be accessible from the outside world

	sc_core::sc_out<bool> irq_out;


	void slave_raiseIrq(){ 
		this->Irq=true;
	};

	void slave_ackIrq() {
		this->Irq=false;
	};

	bool Written;
	bool Irq;

	protected:

	uint32_t slave_read(uint32_t ADDR) {
		return (reg.read());
	};
	
	uint32_t slave_write(uint32_t ADDR, uint32_t DATA) {
		//Acknowledge any pending IRQ
		slave_ackIrq();
		Written=true;
		reg = DATA;
		return 0xDE;
	};

	private:
	void transition() {
			this->irq_out = Irq;
			this->WbSlaveModule<wb_param>::transition();
		}


    };
}
}



#endif
