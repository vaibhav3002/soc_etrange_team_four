
#include <cstdio>

#include <fstream>
#include <sstream>
#include <iomanip>

#include "video_out_master.h"
#include "../segmentation.h"

#define tmpl(x) template<typename wb_param> x Video_out_Master<wb_param>

namespace soclib { namespace caba {

   tmpl(/**/)::Video_out_Master (sc_module_name name )
      :sc_core::sc_module(name), p_clk("p_clk"),p_resetn("p_resetn"), master1(p_clk_100mhz,p_wb)

   {
      // sc thread
      SC_THREAD(process_load_buffer_from_ram);
      // no sensitivity list, waits have to be explicit
      SC_THREAD(process_buffer_out);
   }

   // write buffer process
   tmpl(void)::process_load_buffer_from_ram()
   { 
      bool reset_done;
      // handle a reset cmd
      uint32_t index_8;
      uint32_t index;
      uint32_t * buffer_pnt;
      uint32_t mem;
      bool initial_write;
      for (;;) {

         if (!p_resetn) // rese
         {
            reset_done = true;
            index=0;
            master1.reset();
            index_8=0;
            mem=0x41000000;
            data[0]=0;
            buffer_pnt=data;
            start_reading=false;
            initial_write=true;
            writes_count=0;
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
            if (start_loading){
               //std::cout<<writes_count<<reads_count<<std::endl;
               if ((writes_count!=reads_count) || initial_write ){           //first time we will load data immediatly. After that we will write only once          
                                                                             //a vlock of data has been sent to the output
               master1.wb_read_blk ( mem , VIDEO_OUT_BLOCK_SIZE , buffer_pnt); 
        //       std::cout<<"wrote a block"<<std::endl;
               writes_count++;                                              //Once we have loaded data from ram we increase counter
               writes_count=writes_count % VIDEO_OUT_MODULO;
               
               mem=mem+4*(VIDEO_OUT_BLOCK_SIZE);                            //next ram memory adress
               if (mem> RAM_BASE+RAM_SIZE-4*(VIDEO_OUT_BLOCK_SIZE) )
                  mem=0x41000000;
               buffer_pnt=&data[writes_count*VIDEO_OUT_BLOCK_SIZE];         //next buffer block adress
               if(writes_count==0){
               start_reading=true;                                          //The first time buffer gets full we start reading data
               }
               initial_write=false;                                        
               }
            }
         }
         wait(p_clk_100mhz.posedge_event());
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
            wait(p_clk.posedge_event());
        }
#ifdef SOCLIB_MODULE_DEBUG
        std::cout
            << name()
            << " wait end @" << sc_time_stamp()
            << std::endl;
#endif
    }

    tmpl(void):: process_buffer_out (){
       // handle a reset cmd
       bool reset_true=0;
       uint32_t index;
       uint32_t index_8;
       uint32_t x=0;
       uint32_t y=0;
       for (;;) {

          if (!p_resetn) // reset
          {  
             load_block=true;
             index=0;
             index_8=0;
             reads_count=0;
             pixel_out=false;
             line_valid=false;
             frame_valid=false;
             //  reset_done = true;
             /*
                mem=0x41000000;
                mask=0xF;
                read_count=0;
                index=0;
                index_8=0;

*/
          }
          else // clk event
          {
             // check if the reset has been done
             if (!reset_true)
             {
             }

             // std::cout<<"video out buffer out running<<std"<<std::endl;             
             //write application here
             if (start_reading){
                if ((y<IMAGE_HEIGHT)&&(x>LINE_SYNCH-1)){
                   pixel_out= (uint8_t) ((data[index] & ( 0xFF<<8*index_8 ))>>8*index_8)  ;        //getting 8 bits value 
                                                                                                   //out of a 32 bit buffer
                   index_8++;
                   if ((index_8 % 4 ==0) && (index_8!=0))
                      index++;


                   if (((index % (VIDEO_OUT_BLOCK_SIZE))==0) && (index!=0)&& (index_8 % 4==0) ){
                      reads_count++; 
                      reads_count=reads_count % VIDEO_OUT_MODULO;

          //            std::cout<<"read a block"<<std::endl;
                   }
                }
                else
                   pixel_out = 0xBB;

                line_valid = (y<IMAGE_HEIGHT) && (x>LINE_SYNCH-1);
                frame_valid = (y<IMAGE_HEIGHT);
                index= index==VIDEO_OUT_WINDOW_SIZE? 0:index;
                
                if (x==(IMAGE_WIDTH+LINE_SYNCH-1)){
                   y++;
                   x=0;
                }
                else
                   x++;

                if (y==(IMAGE_HEIGHT+FRAME_SYNCH)){
                   x=0;
                   y=0;
                }

             }

          }
          // wait rising edge of clk
          wait(p_clk.posedge_event());
       }      //thread that sends buffer data to ram
    }



}
}// namespace

