//TODO: terminer l'interface avec Slave : creer un module avec heritage et un registre de config, puis l'inclure ici et interfacer le tout

/*
 * =============================================================================
 *
 *       Filename:  video_in.cpp
 *
 *    Description:  WB master video in
 *
 *         Author:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */


#include <cstdio>

#include <fstream>
#include <sstream>
#include <iomanip>

#include "video_in.h"
#include <video_gen.h>
#include "segmentation.h"


#define tmpl(x) template<typename wb_param> x VideoInModule<wb_param>



namespace soclib { namespace caba {


	template<typename wb_param>
		void VideoInModule<wb_param>::clk_pix_event() {
			if (frame_valid==0) {
				//ACTIVATE WRITE SEQUENCE
				write=true;

				end_frame=true;
			}
			else if (line_valid) {
				pix_fifo->write(pixel_in.read());

				if (2*pix_fifo->num_available()>=BUFFER_SIZE) {
					
					if (end_frame) {
						//current frame's first line : resetting counter values and parameters
						end_frame=false;
						offset=0;
					}

					//ACTIVATE WRITE SEQUENCE
					write=true;
				}
			}
			wait();

		}

	template<typename wb_param> 
		void VideoInModule<wb_param>::clk_wb_event() {
			if (write) {
				write=false;
				unsigned char local_buffer[BUFFER_SIZE];
				long idx = 0;
				//CREATE A LOCAL COPY OF THE DATA TO BE TRANSFERED
				while (end_frame && (pix_fifo->num_available()>0)) {
					while (pix_fifo->num_available()>4) {
						for (int i=0; i<4; i++) {
							local_buffer[idx+i] = pix_fifo->read();
						}
						idx+=4;
					}
				}
				offset+= idx/4;
				master0.wb_write_blk(RAM_BASE+offset-idx/4,(uint32_t*) local_buffer, idx/4);
				
			}
			wait();
		}



	tmpl(/**/)::VideoInModule (sc_module_name name )
		:sc_core::sc_module(name), p_clk("p_clk"),p_resetn("p_resetn"), master0(p_clk,p_wb), slave(p_clk,p_resetn,p_wb2)
	{
		
		pix_fifo = new sc_fifo<unsigned char>(BUFFER_SIZE);

		// sc thread
		SC_THREAD(clk_pix_event);
		sensitive << p_clk_pix.pos();
		dont_initialize();

		SC_THREAD(clk_wb_event);
		sensitive << p_clk.pos();
		dont_initialize();
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
			wait(p_clk.posedge_event());
		}
#ifdef SOCLIB_MODULE_DEBUG
		std::cout
			<< name()
			<< " wait end @" << sc_time_stamp()
			<< std::endl;
#endif
	}

}}// namespace

