#include "wb_simple_slave.h"
#include <iostream>

namespace soclib { namespace caba {
    // Constructor
    template <typename wb_param> \
        WbSimpleSlave<wb_param>::WbSimpleSlave(sc_core::sc_module_name insname) : 
            sc_core::sc_module(insname) {
                std::cout << "WishBone simple slave "  << name() 
                    << " created sucsesfully" << std::endl;

                SC_METHOD(transition);
                sensitive << p_clk.pos();

                SC_METHOD(genMealy);
                sensitive << p_clk.neg();
                sensitive << p_wb;
            }


    // Synchronoue methode
    template <typename wb_param> \
        void WbSimpleSlave<wb_param>::transition() {

            if (p_resetn == false) {
                // reset cycle couter
                cycle = 0;
                // reset write requests counter
                w_req_cpt = 0;
#ifdef SOCLIB_MODULE_DEBUG
                std::cout << name() << " "
                    << "RESET:: waiting for requests"
                    << std::endl;
#endif
                return;
            }
            cycle++;
            if (p_wb.STB_I ) {
                if (p_wb.CYC_I ) {
                    std::cout << name() << " "
                        << "Recived a valid strobe" << std::endl
                        << p_wb
                        << " at cycle "<< std::dec << cycle
                        << std::endl;
                    if (p_wb.WE_I) {
                        w_req_cpt++; 
                    }
                }
                else  {
                    std::cout << name() << " "
                        << "Recived an invalid strobe" << std::endl
                        << p_wb
                        << " at cycle "<< std::dec << cycle
                        << std::endl;
                }
            }
        }

    // Synchronoue methode
    template <typename wb_param> \
        void WbSimpleSlave<wb_param>::genMealy() {
            // always acknowledge requests
            p_wb.ACK_O = p_wb.STB_I && p_wb.STB_I;
            // read write request counter
            p_wb.DAT_O = w_req_cpt ;
        }
}}
