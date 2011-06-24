#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "interpolator.h"


namespace soclib { namespace caba {

    interpolator ::interpolator (sc_module_name name )
	:sc_module(name)
    {
	// sc thread
	SC_THREAD(interpolate);
	// no sensitivity list, waits have to be explicit
    }


    void interpolator:: interpolate(){
	// handle a reset cmd
	bool reset_done = false;
	state mystate=calc_pixel_00;
	for (;;) {

	    if (!p_resetn) // reset
	    {
		reset_done = true;
		pixel_out_valid=false;
		accumulated_result=0;
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
		    wait(p_clk.posedge_event());
		    continue;
		}

		//interpolator code here   
		if (pixel_valid){
	
		    if((dx==0) && (dy==0)){
			pixel_out=pixel_0;
			pixel_out_valid=true;
		    }
		    else
		    {
			accumulated_result=(((~dx & 0xFF )+1)*((~dy & 0xFF )+1)* (pixel_0 )) ;

			accumulated_result=accumulated_result+((((~dx & 0xFF)+1)*(dy)* (pixel_1)));
			accumulated_result=accumulated_result+((dx)*((~dy & 0xFF )+1)*(pixel_2));
			accumulated_result=accumulated_result+ (dx*dy* pixel_3);
			pixel_out=accumulated_result>>16;
			pixel_out_valid=true; 

		    }


	    }else 
		pixel_out_valid=false;


	}

	wait(p_clk.posedge_event());
    }
    // wait rising edge of clk
}      //thread that sends buffer data to ram

}
}// namespace

