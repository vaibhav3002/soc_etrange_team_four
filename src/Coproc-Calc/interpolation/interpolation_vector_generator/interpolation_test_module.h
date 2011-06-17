#ifndef __INTERPOLATOR_TEST_MODULE__
#define __INTERPOLATOR_TEST_MODULE__

#include <systemc.h>
namespace soclib { namespace caba {

    class testinterpolate: sc_core::sc_module
    {
        public:
	sc_core::sc_in<bool> p_resetn;
        sc_core::sc_out<bool> pixel_valid;
	sc_core::sc_out<unsigned char>pixel_out;
	sc_core::sc_in<bool> p_clk_100mhz;
	
	sc_core::sc_out<unsigned char>dx;
	sc_core::sc_out<unsigned char>dy;
private:
	 enum state {single_pixel,pixel00,pixel01,pixel10,pixel11};
            // Interpolation procedure 
	void create_pixels();            
        protected:
            SC_HAS_PROCESS (testinterpolate);
        public:
            // constructor
          testinterpolate( sc_module_name name );
    };
}};

#endif //__INTERPOLATOR__

