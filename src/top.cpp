/*
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 *
 * This file is part of SoCLib, GNU LGPLv2.1.
 *
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 *
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * SOCLIB_LGPL_HEADER_END
 *
 */


// C/C++ std libs
#include <iostream>
#include <cstdlib>
#include <cstdio>

// SocLib
#include "mapping_table.h"
#include "vci_ram.h"

#include "wb_signal.h"

// WB interconnect
#include "wb_interco.h"

// WB->VCI wrappers
#include "wb_slave_vci_initiator_wrapper.h"

// locals
#include "segmentation.h"

//#include "master.h"
#include "video_gen.h"
#include "display.h"
#include "video_in.h"
#include "video_out.h"

//dummy read
#include "master_dummy.h"

//dummy write
#include "master_dummy_write.h"

#define VIDEO_IN 1
#define VIDEO_OUT 1
#define DUMMY_COPRO 0
//#define DO_TRACES

// SystemC main
int _main(int argc, char *argv[])
{
    using namespace sc_core;
    using namespace soclib::caba;

    // Gloabal signals
    sc_time     video_clk_periode(40, SC_NS); // clk period
    sc_clock	video_signal_clk("video_clk",video_clk_periode);
    sc_time     sys_clk_periode(10, SC_NS); // clk period
    sc_clock	sys_signal_clk("system_clk",sys_clk_periode);

    // Define our VCI parameters
    typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

    // Define our WB parameters
    typedef soclib::caba::WbParams<32,32> wb_param;

    soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x80000000);
    maptab.add(Segment("ram" , RAM_BASE , RAM_SIZE , IntTab(0), true));


    // interconnection signals
    soclib::caba::VciSignals<vci_param> signal_vci_ram("signal_vci_vciram");
    
    // WB interconnect
    // sc_name    maptab  masters slaves
    soclib::caba::WbInterco<wb_param> wbinterco("wbinterco",maptab, 4,1);


    // WB interconnect signals
    soclib::caba::WbSignal<wb_param> signal_wb_ram("signal_wb_ram");
    soclib::caba::WbSignal<wb_param> signal_wb_video_in ("signal_wb_video_in");
    soclib::caba::WbSignal<wb_param> signal_wb_video_out ("signal_wb_video_out");
    
    sc_signal<bool>        start_video_out("start_video_out");

    sc_signal<bool> signal_resetn("signal_resetn");

    sc_signal<bool>        line_valid("line_val");
    sc_signal<bool>        frame_valid("frame_val");

    sc_signal<unsigned char> pixel("pixel_val");

    sc_signal<bool>        display_line_valid("display_line_val");
    sc_signal<bool>        display_frame_valid("display_frame_val");

    sc_signal<unsigned char> display_pixel("display_pixel_val");
    
	soclib::caba::WbSignal<wb_param> signal_wb_dummy("wb_dummy"); 
    soclib::caba::WbSignal<wb_param> signal_wb_dummy_write("wb_dummy_write"); 


    VideoGen my_videogen ("video_gen");

    my_videogen.clk (video_signal_clk);
    my_videogen.reset_n(signal_resetn);
    my_videogen.line_valid(line_valid);
    my_videogen.frame_valid(frame_valid);
    my_videogen.pixel_out(pixel);
    
    Display my_display ("My_display");

    my_display.clk (video_signal_clk);
    my_display.reset_n(signal_resetn);
    my_display.line_valid(display_line_valid);
    my_display.frame_valid(display_frame_valid);
    my_display.pixel_in(display_pixel);

#if DUMMY_COPRO
	//dummy read instanciation
	soclib::caba::Master_dummy <wb_param>    master_dummy  ("master_dummy");
	master_dummy.p_clk_100mhz(sys_signal_clk);
	master_dummy.p_resetn(signal_resetn);
	master_dummy.p_wb(signal_wb_dummy);
	master_dummy.p_clk(video_signal_clk);

	//dummy write instanciation
	soclib::caba::Master_dummy_write <wb_param>    master_dummy_write  ("master_dummy_write");
	master_dummy_write.p_clk_100mhz(sys_signal_clk);
	master_dummy_write.p_resetn(signal_resetn);
	master_dummy_write.p_wb(signal_wb_dummy_write);
	master_dummy_write.p_clk(video_signal_clk);
#endif

    // memories
    soclib::caba::VciRam<vci_param> ram("ram", IntTab(0), maptab);
    // Maitre simple
//    soclib::caba::Master<wb_param> wbmaster("master");
    // ram
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> ram_w ("ram_w") ;
    ram_w.p_clk               (sys_signal_clk);
    ram_w.p_resetn            (signal_resetn);
    ram_w.p_vci               (signal_vci_ram);
    ram_w.p_wb                (signal_wb_ram);

    ////////////////////////////////////////////////////////////
    ///////////////////// WB Net List //////////////////////////
    ////////////////////////////////////////////////////////////
#if 0
    wbmaster.p_clk   (sys_signal_clk);
    wbmaster.p_resetn(signal_resetn);
    wbmaster.p_wb    (signal_wb_m);
#endif
    wbinterco.p_clk(sys_signal_clk);
    wbinterco.p_resetn(signal_resetn);

    wbinterco.p_from_master[0](signal_wb_video_in);
    wbinterco.p_from_master[1](signal_wb_video_out);
    wbinterco.p_from_master[2](signal_wb_dummy);
    wbinterco.p_from_master[3](signal_wb_dummy_write);

    wbinterco.p_to_slave[0](signal_wb_ram);
#if VIDEO_IN
    soclib::caba::Video_in<wb_param> video_in("video_in");

    ////////////////////////////////////////////////////////////
    /////////////////// WB -> VCI Wrappers /////////////////////
    ////////////////////////////////////////////////////////////

    video_in.p_clk   (sys_signal_clk);
    video_in.p_resetn(signal_resetn);
    video_in.p_wb    (signal_wb_video_in);
    video_in.start_video_out (start_video_out);
    
    video_in.pixel_clk (video_signal_clk);
    video_in.reset_n(signal_resetn);
    video_in.line_valid(line_valid);
    video_in.frame_valid(frame_valid);
    video_in.pixel_in(pixel);
#endif
#if VIDEO_OUT
    soclib::caba::Video_out<wb_param> video_out("video_out");

    ////////////////////////////////////////////////////////////
    /////////////////// WB -> VCI Wrappers /////////////////////
    ////////////////////////////////////////////////////////////

    video_out.p_clk   (sys_signal_clk);
    video_out.p_resetn(signal_resetn);
    video_out.p_wb    (signal_wb_video_out);
    video_out.start_video_out (start_video_out);
    
    video_out.pixel_clk (video_signal_clk);
    video_out.reset_n(signal_resetn);
    video_out.line_valid(display_line_valid);
    video_out.frame_valid(display_frame_valid);
    video_out.pixel_out(display_pixel);
#endif
    ////////////////////////////////////////////////////////////
    //////////////////// VCI Net List //////////////////////////
    ////////////////////////////////////////////////////////////

    // ram
    ram.p_clk(sys_signal_clk);
    ram.p_resetn(signal_resetn);
    ram.p_vci(signal_vci_ram);

    ////////////////////////////////////////////////////////////
    //////////////// Traces ////////////////////////////////////
    ////////////////////////////////////////////////////////////

#ifdef DO_TRACES
    sc_trace_file *TRACEFILE;
    TRACEFILE = sc_create_vcd_trace_file("vcd_traces");
    sc_trace (TRACEFILE, signal_resetn,  "resetn"  );
    sc_trace (TRACEFILE, sys_signal_clk,     "clk"     );
    sc_trace (TRACEFILE, video_signal_clk, "pixel_clk");
    sc_trace (TRACEFILE, pixel, "pixel_in");
    sc_trace (TRACEFILE, display_pixel, "pixel_out");
//   sc_trace (TRACEFILE, signal_wb_m,    "wb_m");
//  sc_trace (TRACEFILE, signal_vci_ram, "ram");
#endif

    ////////////////////////////////////////////////////////////
    //////////////// Start Simulation //////////////////////////
    ////////////////////////////////////////////////////////////

#if 0
 // reset
    sc_start(sys_clk_periode);
    signal_resetn = true;
    sc_start(sys_clk_periode);
    signal_resetn = false;
    sc_start(sys_clk_periode);
    signal_resetn = true;

    // print maping table
    std::cout << "Let's go..." << std::endl;
    std::cout << maptab;

    // run SystemC simulator
    // sc_start(sc_core::sc_time(1000, SC_NS));
    sc_start();
#endif

#if 1
    /* Initialisation de la simulation */
    signal_resetn = true;
    for (int i=0; i< 10; i++)
      sc_start(sys_clk_periode);

    /* Generation d'un reset */
    signal_resetn = false;
    for (int i=0; i< 20; i++)
      sc_start(sys_clk_periode);
    signal_resetn = true;

    // run SystemC simulator
#ifdef MTI_SYSTEMC
    sc_start();
#else
    sc_start(sc_time(500, SC_MS));
#endif

#endif
#if 0
    // reset
    signal_resetn = true;
    std::cout << "Reset phase" << std::endl;
    sc_start(sc_core::sc_time(0, SC_NS));
    signal_resetn = false;
    sc_start(sc_core::sc_time(1, SC_NS));
    signal_resetn = true;

    // print maping table
    std::cout << "Let's go..." << std::endl;
    std::cout << maptab;

    // run SystemC simulator
    // sc_start(sc_core::sc_time(1000, SC_NS));
    sc_start( );
#endif
#ifdef DO_TRACES
    sc_close_vcd_trace_file(TRACEFILE);
#endif


    return EXIT_SUCCESS;
}

int sc_main(int argc, char *argv[])
{
    try {
        return _main(argc, argv);
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "Unknown exception occured" << std::endl;
        throw;
    }
    return 1;
}
