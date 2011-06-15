#ifndef __INTERPOLATOR__
#define __INTERPOLATOR__

#include <systemc.h>
#define FRACTIONAL_PART_BIT_SIZE  unsigned char
namespace soclib { namespace caba {

    class interpolator: sc_core::sc_module
    {
        public:

            sc_core::sc_in<unsigned char> pixel_in ;
            sc_core::sc_in<FRACTIONAL_PART_BIT_SIZE>dx;
            sc_core::sc_in<FRACTIONAL_PART_BIT_SIZE>dy;
            sc_core::sc_out<unsigned char>pixel_out;
            sc_core::sc_out<bool>pixel_out_valid;
            
            sc_core::sc_in<bool> pixel_valid;
            sc_core::sc_in<bool>p_resetn;
            sc_core::sc_in<bool> p_clk;
       
        private:
            enum state {calc_pixel_00,calc_pixel_01,calc_pixel_10,calc_pixel_11};
           long long unsigned int accumulated_pixel_result;

            // Interpolation procedure 
            void interpolate();
            
        protected:
            SC_HAS_PROCESS (interpolator);
        public:
            // constructor
          interpolator( sc_module_name name );
    };
}};

#endif //__INTERPOLATOR__

