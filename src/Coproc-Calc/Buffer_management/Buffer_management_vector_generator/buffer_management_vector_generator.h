#ifndef __BUFFER_MANAGEMENT_TEST_MODULE__
#define __BUFFER_MANAGEMENT_TEST_MODULE__

#include <systemc.h>
namespace soclib { namespace caba {

    class testbuffermanagement: sc_core::sc_module
    {
        public:
	sc_core::sc_in<bool> p_resetn;
	sc_core::sc_in<bool> p_clk_100mhz;
	sc_core::sc_out<bool> pixel_valid;
	sc_core::sc_out<uint16_t>dx;
	sc_core::sc_out<uint16_t>dy;
	sc_core::sc_out<unsigned char>x;
	sc_core::sc_out<unsigned char>y;
private:
	 enum state {single_pixel,pixel00,pixel01,pixel10,pixel11};
            // Interpolation procedure 
	void create_pixels();            
        protected:
            SC_HAS_PROCESS (testbuffermanagement);
        public:
            // constructor
          testbuffermanagement( sc_module_name name );
    };
}};

#endif //__BUFFER_MANAGEMENT_TEST_MODULE__

