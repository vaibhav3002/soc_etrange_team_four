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
		    written = 0;
		    read = 0;
		}

	sc_signal<uint32_t> reg_p0; //Internal register that must be accessible from the outside world
	sc_signal<uint32_t> reg_s0;
	sc_signal<uint32_t> reg_s1;
	sc_signal<uint32_t> reg_r0;
	sc_signal<uint32_t> reg_r1;
	sc_signal<uint32_t> reg_r2;
	sc_signal<uint32_t> reg_q0;
	sc_signal<uint32_t> reg_q1;
	sc_signal<uint32_t> reg_q2;
	sc_signal<uint32_t> reg_q3;
	int written;
	int read;
	sc_signal<bool> ready;
	sc_signal<bool> loaded;
	protected:


	uint32_t slave_read(uint32_t ADDR) {
	    switch(ADDR)
	    {
	    case 0:
		return (reg_p0.read());
		break;
	    case 1:
		return (reg_s0.read());
		break;
	    case 2:
		return (reg_s1.read());
		break;
	    case 3:
		return (reg_r0.read());
		break;
	    case 4:
		return (reg_r1.read());
		break;
	    case 5:
		return (reg_r2.read());
		break;
	    case 6:
		return (reg_q0.read());
		break;
	    case 7:
		return (reg_q1.read());
		break;
	    case 8:
		return (reg_q2.read());
		break;
	    case 9:
		return (reg_q3.read());
		break;
	    }	
	    read++;
	    if (read==10)
	    {
		loaded = 1;
		read = 0;
	    }
	    
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
		reg_p0 = DATA;
		break;
	    case 1:
		reg_s0 = DATA;
		break;
	    case 2:
		reg_s1 = DATA;
		break;
	    case 3:
		reg_r0 = DATA;
		break;
	    case 4:
		reg_r1 = DATA;
		break;
	    case 5:
		reg_r2 = DATA;
		break;
	    case 6:
		reg_q0 = DATA;
		break;
	    case 7:
		reg_q1 = DATA;
		break;
	    case 8:
		reg_q2 = DATA;
		break;
	    case 9:
		reg_q3 = DATA;
		break;
	    }
	    written++;
	    if (written==10)
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
