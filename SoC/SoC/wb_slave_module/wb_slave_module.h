/*
 * =============================================================================
 *
 *       Filename:  wb_slave_module.h
 *
 *    Description:  generic wb slave module
 *
 *	   Author:  Thibault Porteboeuf
 *         Author:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef _WB_SLAVE_MODULE_
#define _WB_SLAVE_MODULE_


#include <iostream>

#include <systemc>

#include "wb_master.h"

namespace soclib { namespace caba {

    template<typename wb_param>
        class WbSlaveModule
    {
            protected:
                SC_HAS_PROCESS(WbSimpleSlave);

        public:

            void reset ();
            void print_stats();

            // constructor
            WbSlaveModule ( sc_core::sc_in<bool> &p_clk,
                    soclib::caba::WbSlave<wb_param> &p_wb,
		    sc_core::sc_in<bool> &p_resetn
                    );

            friend std::ostream& operator<< (std::ostream &o, WbSlaveModule &wbm)
            {
                wbm._print(o);
                return o;
            }

        private:

            // port are private and should be provided  as constructor arguments
            sc_core::sc_in<bool> &p_clk;
            soclib::caba::WbSlave<wb_param> &p_wb;
	    sc_core::sc_in<bool> &p_resetn;

            uint32_t cycme;
            uint32_t w_req_cpt;

	    void transition();
	    void genMealy();



            inline void _print (std::ostream &o)
            {
                o
                    << std::dec
                    << "Cycles " << cycle
                    << std::endl
                    << "Write requests " << w_req_cpt;
            }

    };
}}


#endif //_WB_SLAVE_MODULE_
