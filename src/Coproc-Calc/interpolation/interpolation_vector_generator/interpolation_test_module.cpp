/*:v
 * =============================================================================
 *
 *       Filename:  Interpolation test module.cpp
 *
 *    Description:  Interpolation test module
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
#include "interpolation_test_module.h"
#include "../../../segmentation.h"


namespace soclib { namespace caba {

   testinterpolate::testinterpolate (sc_module_name name )
      :sc_module(name)

   {
      // sc thread
      // no sensitivity list, waits have to be explicit
      SC_THREAD(create_pixels);
   }

   // write buffer process
   void testinterpolate::create_pixels()
   {
      bool reset_done;
      state mystate;
      unsigned char saved_dx,saved_dy;
      for (;;) {

         if (!p_resetn) // rese
         {
            reset_done = true;
            pixel_valid=false; 
	    mystate=single_pixel;
	    pixel_out=0;
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

         
	switch(mystate){
		
		     case single_pixel:
				  dx=0;
				  dy=0;
				  pixel_out=pixel_out+10;
				  pixel_valid=true;
				  mystate=pixel00;
		     break;
			
		     case pixel00:
				
				 // dx=saved_dx+1;
				  dx=0;
				  dy=saved_dy+1;
				  pixel_out=pixel_out+10;
				  pixel_valid=true;
				  mystate=pixel01;
 		     break;
			
		     case pixel01:
				
				  pixel_out=pixel_out+10;
				  mystate=pixel10;
		     break;		 

		     case pixel10:

				  pixel_out=pixel_out+10;	
				  mystate=pixel11;
		     break;
			
		     case pixel11:
				
				  pixel_out=pixel_out+10;
				  saved_dx=dx;
				  saved_dy=dy;
				  mystate=single_pixel;
		     break;
			
		     default:
					//debug here
							;
			} 

	}

         //         std::cout<< read_count <<"read_count"<< write_count <<"write_count"<< std::endl;
         wait(p_clk_100mhz.posedge_event());                                                     //next clock
      }

   } // infinite loop


}
}// namespace

