/*
 * =============================================================================
 *
 *       Filename:  video_in.h
 *
 *    Description:  video_in module
 *		    based on the generic wb master module
 *
 *         Author:  Thibault Porteboeuf, thibault.porteboeuf@telecom-paristech.fr
 *	   Credit:  Tarik Graba (TG), tarik.graba@telecom-paristech.fr
 *        Company:  Telecom Paris TECH
 *
 * =============================================================================
 */


#ifndef _VIDEO_IN_MODULE_
#define _VIDEO_IN_MODULE_


#include <iostream>

#include <systemc>

#include "wb_master.h"

#define BUFFER_SIZE 512

namespace soclib { namespace caba {

	template<typename wb_param>
		class VideoInModule
		{
		public:

		//IO PORTS (Video_gen interface)
			sc_in_clk clk_pix;
			sc_in<bool> reset_n;

			sc_in<bool> line_valid;
			sc_in<bool> frame_valid;
			sc_in<unsigned char> pixel_in;

		////////////////////////////////////////////////////////////////////
		// single read/write operation
		////////////////////////////////////////////////////////////////////
				uint32_t wb_read_at  (
						uint32_t addr
						);

				void     wb_write_at (
						uint32_t addr,
						uint8_t mask,
						uint32_t data
						);

		////////////////////////////////////////////////////////////////////
		// burst read/write operation to/from an uint32_t array
		////////////////////////////////////////////////////////////////////
				void     wb_read_blk (
						uint32_t saddr,
						uint32_t num ,
						uint32_t *dest
						);

				void     wb_write_blk (
						uint32_t saddr,
						uint32_t *data,
						uint32_t num
						);

				void reset ();
				void print_stats();

				// constructor
				VideoInModule ( sc_core::sc_in<bool> &p_clk,
						soclib::caba::WbMaster<wb_param> &p_wb
					      );

				friend std::ostream& operator<< (std::ostream &o, VideoInModule &wbm)
				{
					wbm._print(o);
					return o;
				}

			private:


		//Buffer
				unsigned char Buffer[BUFFER_SIZE];
				long pos;
				long offset;
				bool write;
				bool end_frame;

		// port are private and should be provided  as constructor arguments
				sc_core::sc_in<bool> &p_clk;
				soclib::caba::WbMaster<wb_param> &p_wb;

		// simulation cycle
				uint32_t trans_wait_cycles;
				uint32_t num_writes;
				uint32_t num_reads;

		//Pixel rec fnct
				void clk_pix_event();

		//WB snd fnct
				void clk_wb_event();

		// wait WB ACK
				inline void     WaitWbAck()
				{
					do {
						trans_wait_cycles ++;
						sc_core::wait(p_clk.posedge_event());
				// error in cycle
						if (p_wb.ERR_I) {
							std::cout
						//<< name()
								<< " : Error in WB transaction @ cycle #"
								<< sc_core::sc_time_stamp()
								<< std::endl;
							break;
						}

					} while (!p_wb.ACK_I);

#ifdef SOCLIB_MODULE_DEBUG
					std::cout
				//<< name()
						<< " ACK received @"
						<< sc_core::sc_time_stamp()
						<< std::endl;
#endif

				};

		// Clean WB requests
				inline void     CleanWb()
				{
					p_wb.STB_O  = false;
					p_wb.CYC_O  = false;
					p_wb.LOCK_O = false;
					p_wb.WE_O   = false;
					p_wb.ADR_O  = 0xBadeC0de;
					p_wb.DAT_O  = 0xDeadBeef;
					p_wb.SEL_O  = 0;
				};

				inline void _print (std::ostream &o)
				{
					o
						<< std::dec
						<< "Wait cycles " << trans_wait_cycles
						<< std::endl
						<< " for " << num_writes << " writes and "
						<< num_reads << " reads";
				}


				SC_HAS_PROCESS(VideoInModule);

		};
}}


#endif //_VIDEO_IN_MODULE_
