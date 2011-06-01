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

#include "master_dummy_write.h"
#include "../segmentation.h"

#define tmpl(x) template<typename wb_param> x Master_dummy_write<wb_param>

namespace soclib { namespace caba {

   tmpl(/**/)::Master_dummy_write (sc_module_name name )
      :sc_core::sc_module(name), p_clk("p_clk"),p_resetn("p_resetn"), master1(p_clk_100mhz,p_wb)

   {
      // sc thread
      SC_THREAD(process_write_to_ram);
      // no sensitivity list, waits have to be explicit
   }


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

   tmpl(void):: process_write_to_ram (){
      // handle a reset cmd
      bool reset_done = false;         
      uint32_t mem;
      uint8_t mask[COPRO_OUT_BLOCK_SIZE] ;
      uint8_t * mask_pnt;
      uint32_t data[COPRO_OUT_BLOCK_SIZE];
      uint32_t * buffer_pnt;
      for (;;) {

         if (!p_resetn) // reset
         {
            master1.reset();
            reset_done = true;
            buffer_pnt=data;
            mem=0x400F0000;

            for (int i=0;i<(int) COPRO_OUT_BLOCK_SIZE;i++){  //initialising mask array
               mask[i]=0xFF;
            }
            mask_pnt=mask;                                   //initialising pointer to array
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
               wait(p_clk_100mhz.posedge_event());
               continue;
            }
            master1.wb_write_blk(mem,mask_pnt,buffer_pnt,COPRO_OUT_BLOCK_SIZE); 
         }
         // wait rising edge of clk
         wait(p_clk_100mhz.posedge_event());
      }      //thread that sends buffer data to ram
   }



}
}// namespace

