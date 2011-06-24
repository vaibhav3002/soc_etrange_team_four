/*:v
 * =============================================================================
 *
 *       Filename:  Buffer management vector generator.cpp
 *
 *    Description:  Buffer management vector generator
 *                  This module demonstrate WbMasterModule usage
 *
 *         Author:  Ted
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */


#include <cstdio>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include "buffer_management_vector_generator.h"


namespace soclib { namespace caba {

   testbuffermanagement::testbuffermanagement (sc_module_name name )
      :sc_module(name)

   {
      // sc thread
      // no sensitivity list, waits have to be explicit
      SC_THREAD(create_pixels);
   }

   // write buffer process
   void testbuffermanagement::create_pixels()
   {
      bool reset_done;
      state mystate;
      unsigned char saved_dx,saved_dy,saved_x,saved_y;
      for (;;) {

         if (!p_resetn) // rese
         {
            reset_done = true;
            pixel_valid=false; 
	}
         else // clk event
         {
            // check if the reset has been done
            if (!reset_done)
            {
#ifdef SOCLIB_MODULE_DEBUG
               std::cout
                  << name()
                  << " : has not been reseted yet!!!"
                  << std::endl;
#endif
         		wait(p_clk_100mhz.posedge_event());                                                     //next clock
               continue;
            }
	    dx=0x0;
	    dy=0x0;
	    x=10;
	    y=10;
	    pixel_valid=true;

	}

         //         std::cout<< read_count <<"read_count"<< write_count <<"write_count"<< std::endl;
         wait(p_clk_100mhz.posedge_event());                                                     //next clock
      }

   } // infinite loop


}
}// namespace

