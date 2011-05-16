#include "wb_slave_module.h"
#include <iostream>

namespace soclib { namespace caba {
	// Constructor
	template <typename wb_param> \
		WbSlaveModule<wb_param>::WbSlaveModule(sc_core::sc_in<bool> &p_clk,
				sc_core::sc_in<bool> &p_resetn,
				soclib::caba::WbSlave<wb_param> &p_wb
				): p_clk(p_clk), p_resetn(p_resetn), p_wb(p_wb)
		{

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
#ifdef SOCLIB_MODULE_DEBUG
					std::cout << name() << " "
						<< "Recived a valid strobe" << std::endl
						<< p_wb
						<< " at cycle "<< std::dec << cycle
						<< std::endl;
#endif
					if (p_wb.WE_I) {
						//Write request
						w_req_cpt++;
						status = Write;
					}	
					else {
						//Read request
						status = Read;
					}
				}
				else  {
					status = Wait;
#ifdef SOCLIB_MODULE_DEBUG
					std::cout << name() << " "
						<< "Recived an invalid strobe" << std::endl
						<< p_wb
						<< " at cycle "<< std::dec << cycle
						<< std::endl;
#endif
				}
			} else {
				status = Wait;
			}
		}

	// Synchronoue methode
	template <typename wb_param> \
		void WbSimpleSlave<wb_param>::genMealy() {
			if (status !=Wait) {
				// always acknowledge requests
				p_wb.ACK_O = p_wb.STB_I && p_wb.STB_I;

				if (status == Read) {
					p_wb.DAT_O = slave_read(p_wb.ADD_I);
				} else {
					p_wb.DAT_O = slave_write(p_wb.ADD_I,p_wb.DAT_I);
				}
			}

			virtual template <typename wb_param> \
				uint32_t WbSimpleSlave<wb_param>::read(uint32_t ADDR) {
					return 0;
				}

			virtual template <typename wb_param> \
				uint32_t WbSimpleSlave<wb_param>::write(uint32_t ADDR,uint32_t DATA) {
					return DATA;
				}
		}}
