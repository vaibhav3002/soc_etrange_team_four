/*
 * =============================================================================
 *
 *       Filename:  wb_slave_module.h
 *
 *    Description:  generic wb slave module
 *
 *	   Author:  Thibault Porteboeuf
 *         Credit:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */

#ifndef _WB_SLAVE_MODULE__
#define _WB_SLAVE_MODULE__


#include <iostream>
#include <systemc>
#include "wb_slave.h"


namespace soclib { namespace caba {

    template<typename wb_param>
        class WbSlaveModule : sc_core::sc_module
    {

        public:

            sc_core::sc_in<bool> p_clk;
            soclib::caba::WbSlave<wb_param> p_wb;
	    sc_core::sc_in<bool> p_resetn;


            void reset ();
            void print_stats();

            // constructor
            WbSlaveModule ( sc_module_name name );

            friend std::ostream& operator<< (std::ostream &o, WbSlaveModule &wbm)
            {
                wbm._print(o);
                return o;
            }


	protected:
	    typedef enum {Wait, Write, Read} Status;

	    SC_HAS_PROCESS(WbSlaveModule);
	    
	    Status status;
            uint32_t cycle;
            uint32_t w_req_cpt;


	    virtual void transition();
	    void genMoore();


	    virtual uint32_t slave_read(uint32_t ADDR);
	    virtual uint32_t slave_write(uint32_t ADDR, uint32_t DATA);

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
