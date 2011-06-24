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
	    mystate=pixel00;
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
				  dx=0xFFFF;
				  dy=0xFFFF;
				  x=14;
				  y=14;
				  pixel_valid=true;
				  mystate=pixel00;
		     break;
			
		     case pixel00:
				
				  dx=0xFFFF;
				  dy=0xFFFF;
				  saved_x=saved_x+1;
				  saved_y=saved_y+1;
				  x=saved_x;
				  y=saved_y;
				  pixel_valid=true;
				  mystate=pixel01;
				  saved_x= saved_x==15? 0:saved_x;
				  saved_y= saved_y==15? 0:saved_y;
 		     break;
			
		     case pixel01:
				  pixel_valid=false;
				  mystate=pixel10;
		     break;		 

		     case pixel10:
				  pixel_valid=false;
				  mystate=pixel11;
		     break;
			
		     case pixel11:
				  pixel_valid=false;	
				  mystate=pixel00;
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

