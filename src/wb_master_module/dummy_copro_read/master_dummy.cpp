/*:v
 * =============================================================================
 *
 *       Filename:  master.cpp
 *
 *    Description:  WB master
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

#include "master_dummy.h"
#include "../segmentation.h"

#define tmpl(x) template<typename wb_param> x Master_dummy<wb_param>

namespace soclib { namespace caba {

   tmpl(/**/)::Master_dummy (sc_module_name name )
      :sc_core::sc_module(name),p_clk("p_clk"),p_resetn("p_resetn"), master0(p_clk_100mhz,p_wb)

   {
      // sc thread
      // no sensitivity list, waits have to be explicit
      SC_THREAD(process_read_from_ram);
   }

   // write buffer process
   tmpl(void)::process_read_from_ram()
   {
      bool reset_done;
      uint32_t mem;
      uint32_t data[COPRO_IN_BLOCK_SIZE];
      uint32_t * buffer_pnt; 
      for (;;) {

         if (!p_resetn) // rese
         {
            reset_done = true;
            mem=0x400F0000;
            buffer_pnt=data;
            master0.reset();
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
               continue;
            }

          master0.wb_read_blk ( mem , COPRO_IN_BLOCK_SIZE , buffer_pnt); 
         }

         //         std::cout<< read_count <<"read_count"<< write_count <<"write_count"<< std::endl;
         wait(p_clk_100mhz.posedge_event());                                                     //next clock
      }

   } // infinite loop

   // wait cycles
   tmpl(void):: wait_cycles (uint32_t delay)
   {
#ifdef SOCLIB_MODULE_DEBUG
      std::cout
         << name()
         << " wait start @" << sc_time_stamp()
         << std::endl;
#endif
      while (delay--) {
         wait(p_clk_100mhz.posedge_event());
      }
#ifdef SOCLIB_MODULE_DEBUG
      std::cout
         << name()
         << " wait end @" << sc_time_stamp()
         << std::endl;
#endif
   }



}
}// namespace

