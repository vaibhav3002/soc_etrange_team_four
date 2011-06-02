
#ifndef __VIDEO_OUT_MASTER__
#define __VIDEO_OUT_MASTER__

#include <systemc>
#include "wb_master_module.h"
#include "../video_buffer.h"
#include "wb_slave_reg.h"

#define VIDEO_OUT_WINDOW_SIZE  VIDEO_BUFFER_SIZE 
#define VIDEO_OUT_BLOCK_SIZE  VIDEO_BUFFER_BLOCK_SIZE    //here block 
#define VIDEO_OUT_MODULO (VIDEO_OUT_WINDOW_SIZE/VIDEO_OUT_BLOCK_SIZE) //of data to be read from ram is defined  


#define VIDEO_OUT_LINE_VALID  ((uint32_t) 160) 
#define VIDEO_OUT_FRAME_VALID  ((uint32_t) 40)
#define IMAGE_HEIGHT  ((uint32_t) 480)
#define LINE_SYNCH  ((uint32_t) 160)
#define FRAME_SYNCH ((uint32_t) 40)
#define IMAGE_WIDTH  ((uint32_t) 640)



namespace soclib { namespace caba {

    template<typename wb_param>
        class Video_out_Master: sc_core::sc_module
    {
        public:
            
            sc_core::sc_in<bool> p_clk;
            sc_core::sc_in<bool> p_clk_100mhz;
            sc_core::sc_in<bool> p_resetn;
            sc_core::sc_out<bool> line_valid;
            sc_core::sc_out<bool> frame_valid;
            sc_core::sc_out<unsigned char> pixel_out;
            soclib::caba::WbMaster<wb_param> p_wb;
            sc_core::sc_in<bool> start_loading;
            WbSlaveRegModule<wb_param> reg0;
        private:
            
            // send data to ram 
            void process_load_buffer_from_ram();
            void process_buffer_out();
            uint32_t data[VIDEO_OUT_WINDOW_SIZE];
            bool load_block;
            bool start_reading;
            uint32_t * buffer_pnt;
            uint32_t writes_count;
            uint32_t reads_count;
            void     wait_cycles (uint32_t delay);
            WbMasterModule<wb_param> master1; 
        protected:
            SC_HAS_PROCESS(Video_out_Master);

        public:
            // constructor
            Video_out_Master ( sc_module_name name );
    };
}}

#endif //__MASTER__

