#include "wb_simple_master.h"
#include "segmentation.h"
#include <iostream>
#include <stdio.h>

namespace soclib { namespace caba {
    // Constructor
    template <typename wb_param> \
    WbSimpleMaster<wb_param>::WbSimpleMaster(sc_core::sc_module_name insname)
    : sc_core::sc_module(insname)
    {
        std::cout << "WishBone simple Master "  << name() 
            << " created sucsesfully" << std::endl;
        SC_METHOD(transition);
        dont_initialize();
        sensitive << p_clk.pos();
        SC_METHOD(genMoore);
        dont_initialize();
        sensitive << p_clk.neg();
    }

    // transition
    template <typename wb_param> \
       void  WbSimpleMaster<wb_param>::transition() {
            //reset
            if (p_resetn == false) {
            // add initialiszations
                std::cout << name() << " Reset..."<<std::endl;
                return;
            }
            // rising edge of clk
//                std::cout << name() 
//                << " Rising edge of clk"<<std::endl;

		switch (this->State) {
			case Wait:
				top++;
				if (top==2000) {
					this->State=Read;
				}
				if (top==4000) {
					top=0;
					this->State=Write;
				}

				break;
			case Read:
				if (p_wb.ACK_I==1) { 
					this->State=Wait;
					// sample data
					printf("\n > Master read 0x%x\n\n",(int) p_wb.DAT_I.read());
				}
				break;
			case Write:
				if (p_wb.ACK_I==1) { 
					this->State=Wait;
				}
				break;
		}

        }

    // genMoore
    template <typename wb_param> \
        void WbSimpleMaster<wb_param>::genMoore() {
        // on the falling edge of clk
//                std::cout << name() 
//                << " Falling edge of clk"<<std::endl;

		switch (this->State) {
			case Wait:
				p_wb.STB_O = 0;
				p_wb.CYC_O = 0;
				p_wb.WE_O = 0;
				break;
			case Read:
				p_wb.STB_O = 1;
				p_wb.CYC_O = 1;
				p_wb.ADR_O = WBS_BASE;
				break;
			case Write:
				p_wb.WE_O = 1;
				p_wb.STB_O = 1;
				p_wb.CYC_O = 1;
				p_wb.DAT_O = 123;
				p_wb.ADR_O = WBS_BASE;
				break;
		}
        }

}}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
