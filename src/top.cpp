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

//#define DO_TRACES 1  

// C/C++ std libs
#include <iostream>
#include <cstdlib>
#include <cstdio>

// SocLib
#include "mapping_table.h"
#include "iss2_simhelper.h"
#include "lm32.h"
#include "vci_ram.h"
#include "vci_rom.h"
#include "vci_multi_tty.h"

#include "wb_signal.h"

#include "wb_xcache_wrapper.h"

// WB interconnect
#include "wb_interco.h"
// WB->VCI wrappers
#include "wb_slave_vci_initiator_wrapper.h"

// locals
#include "segmentation.h"

//video generator
#include "video_gen.h"

//video_in_master_module
#include "video_in.h"

//dummy read
#include "master_dummy.h"

//dummy write
#include "master_dummy_write.h"

//video_out master module
#include "video_out_master.h"

//display module
#include "display.h"

// real SystemC main
int _main(int argc, char *argv[])
{
    using namespace sc_core;
    // Avoid repeating these everywhere
    using soclib::common::IntTab;
    using soclib::common::Segment;

    // Define our VCI parameters
    typedef soclib::caba::VciParams<4,8,32,1,1,1,8,1,1,1> vci_param;

    // Define our WB parameters
    typedef soclib::caba::WbParams<32,32> wb_param;

    // Mapping table
    soclib::common::MappingTable maptab(32, IntTab(8), IntTab(8), 0x80000000);

    maptab.add(Segment("rom" , ROM_BASE , ROM_SIZE , IntTab(0), true));
    maptab.add(Segment("ram" , RAM_BASE , RAM_SIZE , IntTab(1), true));
    maptab.add(Segment("tty"  , TTY_BASE  , TTY_SIZE  , IntTab(2), false));
    
    // Global signals
    sc_time     clk_periode(10, SC_NS); // clk period
    sc_clock	signal_clk("signal_clk",clk_periode);
    sc_signal<bool> signal_resetn("signal_resetn");
    
    //video_gen signals
    sc_signal<bool>        line_in_valid;   // Les données sont sur une ligne valide
    sc_signal<bool>        frame_in_valid;   // Les données sont sur la partie valide de l'image
    sc_signal<unsigned char> pixel_in;       // La valeur des pixels
   
    //video_afficheur signals
    sc_signal<bool>        line_out_valid;   // Les données sont sur une ligne valide
    sc_signal<bool>        frame_out_valid;   // Les données sont sur la partie valide de l'image
    sc_signal<unsigned char> pixel_out;       // La valeur des pixels
    sc_signal<bool>        start_loading_connection;

   //video_gen to master signals 
   sc_time                 clk25_periode(40, SC_NS);
   sc_clock                signal25_clk("signal25_clk",clk25_periode);        //25mhz clock piwel clock

   // interconnection signals
    soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
    soclib::caba::VciSignals<vci_param> signal_vci_rom("signal_vci_vcirom");
    soclib::caba::VciSignals<vci_param> signal_vci_ram("signal_vci_vciram");

    // WB interconnect signals
    soclib::caba::WbSignal<wb_param> signal_wb_lm32("signal_wb_lm32");
    soclib::caba::WbSignal<wb_param> signal_wb_ram("signal_wb_ram");
    soclib::caba::WbSignal<wb_param> signal_wb_rom("signal_wb_rom");
    soclib::caba::WbSignal<wb_param> signal_wb_tty("signal_wb_tty");
    soclib::caba::WbSignal<wb_param> signal_wb_mastermodule("master");      //Initialising Master wishbone
    soclib::caba::WbSignal<wb_param> signal_wb_video_out_mastermodule("video_out_master");      //Initialising Master wishbone
    soclib::caba::WbSignal<wb_param> signal_wb_dummy("wb_dummy"); 
    soclib::caba::WbSignal<wb_param> signal_wb_dummy_write("wb_dummy_write"); 

    // irq from uart
    sc_signal<bool> signal_tty_irq("signal_tty_irq");
    // unconnected irqs
    sc_signal<bool> unconnected_irq ("unconnected_irq");
    

    // Components
    // lm32 real cache configuration can be:
    // Ways 1 or 2
    // Sets 128,256,512 or 1024
    // Bytes per line 4, 8 or 16
    // Here we have 2 way, 128 set and 8 bytes per set
    // To simulate a processor without a these parameters should be
    // changed to 1,1,4
    soclib::caba::WbXcacheWrapper
        <wb_param, soclib::common::Iss2Simhelper<soclib::common::LM32Iss <false > > >
        lm32("lm32", 0, maptab,IntTab(0), 2,128,8, 2,128,8);

    // elf loader
    soclib::common::Loader loader("soft/soft.elf");

    // memories
    soclib::caba::VciRom<vci_param> rom("rom", IntTab(0), maptab, loader);
    soclib::caba::VciRam<vci_param> ram("ram", IntTab(1), maptab, loader);
    soclib::caba::VciMultiTty<vci_param> vcitty("vcitty",	IntTab(2), maptab, "tty.log", NULL);
    //5 Masters: video_in, video_out, processor, dummy read, dummy write
    //3 slaves, ram, rom, tty
    soclib::caba::WbInterco<wb_param> wbinterco("wbinterco",maptab, 5,3);

    //Video Gen creation and instantation
    soclib::caba::VideoGen  videogen("video_gen");     
    videogen.clk(signal25_clk);
    videogen.line_valid (line_in_valid); 
    videogen.frame_valid(frame_in_valid);
    videogen.reset_n(signal_resetn);
    videogen.pixel_out(pixel_in);
    
    //Video_in Master generation and instantiotion
    soclib::caba::Video_in<wb_param>    master_module  ("video_in");
    master_module.p_clk(signal25_clk);
    master_module.p_clk_100mhz(signal_clk);
    master_module.p_resetn(signal_resetn);
    master_module.line_valid(line_in_valid);
    master_module.frame_valid(frame_in_valid);
    master_module.pixel_in(pixel_in);
    master_module.p_wb(signal_wb_mastermodule);
    master_module.start_loading(start_loading_connection);
    
	//Video_out Master generation and instanciation
	soclib::caba::Video_out_Master <wb_param>    video_out_master_module  ("video_out_master");
	video_out_master_module.p_clk_100mhz(signal_clk );
	video_out_master_module.p_clk(signal25_clk);
	video_out_master_module.p_resetn(signal_resetn);
	video_out_master_module.line_valid(line_out_valid);
	video_out_master_module.frame_valid(frame_out_valid);
	video_out_master_module.pixel_out(pixel_out);
	video_out_master_module.p_wb(signal_wb_video_out_mastermodule);
	video_out_master_module.start_loading(start_loading_connection);

	//display instanciation
	soclib::caba::Display display("display"); 
	display.clk(signal25_clk);
	display.reset_n(signal_resetn);
	display.line_valid(line_out_valid);
	display.frame_valid(frame_out_valid);
	display.pixel_in(pixel_out);
	//dummy read instanciation
	soclib::caba::Master_dummy <wb_param>    master_dummy  ("master_dummy_read");
	master_dummy.p_clk_100mhz(signal_clk);
	master_dummy.p_resetn(signal_resetn);
	master_dummy.p_wb(signal_wb_dummy);
	master_dummy.p_clk(signal25_clk);

	//dummy write instanciation
	soclib::caba::Master_dummy_write <wb_param>    master_dummy_write  ("master_dummy_write");
	master_dummy_write.p_clk_100mhz(signal_clk);
	master_dummy_write.p_resetn(signal_resetn);
	master_dummy_write.p_wb(signal_wb_dummy_write);
	master_dummy_write.p_clk(signal25_clk);

    ////////////////////////////////////////////////////////////
    /////////////////// WB -> VCI Wrappers /////////////////////
    ////////////////////////////////////////////////////////////

    // ram
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> ram_w ("ram_w") ;
    ram_w.p_clk               (signal_clk);
    ram_w.p_resetn            (signal_resetn);
    ram_w.p_vci               (signal_vci_ram);
    ram_w.p_wb                (signal_wb_ram);

    // rom
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> rom_w ("rom_w") ;
    rom_w.p_clk               (signal_clk);
    rom_w.p_resetn            (signal_resetn);
    rom_w.p_vci               (signal_vci_rom);
    rom_w.p_wb                (signal_wb_rom);

    // tty
    soclib::caba::WbSlaveVciInitiatorWrapper<vci_param, wb_param> tty_w ("tty_w") ;
    tty_w.p_clk               (signal_clk);
    tty_w.p_resetn            (signal_resetn);
    tty_w.p_vci               (signal_vci_tty);
    tty_w.p_wb                (signal_wb_tty);

    
    ////////////////////////////////////////////////////////////
    ///////////////////// WB Net List //////////////////////////
    ////////////////////////////////////////////////////////////
    
    wbinterco.p_clk(signal_clk);
    wbinterco.p_resetn(signal_resetn);

    wbinterco.p_from_master[0](signal_wb_lm32);

    wbinterco.p_to_slave[0](signal_wb_rom);
    wbinterco.p_to_slave[1](signal_wb_ram);
    wbinterco.p_to_slave[2](signal_wb_tty);
    wbinterco.p_from_master[1](signal_wb_mastermodule);
    wbinterco.p_from_master[2](signal_wb_video_out_mastermodule);
    wbinterco.p_from_master[3](signal_wb_dummy);
    wbinterco.p_from_master[4](signal_wb_dummy_write);

    // lm32
    lm32.p_clk(signal_clk);
    lm32.p_resetn(signal_resetn);
    lm32.p_wb(signal_wb_lm32);
    // !! the real LM32 irq are active low
    // To avoid adding inverters here, we consider
    // them active high
    lm32.p_irq[0] (signal_tty_irq);
    for (int i=1; i<32; i++)
        lm32.p_irq[i] (unconnected_irq);

    ////////////////////////////////////////////////////////////
    //////////////////// VCI Net List //////////////////////////
    ////////////////////////////////////////////////////////////
   
    // rom
    rom.p_clk(signal_clk);
    rom.p_resetn(signal_resetn);
    rom.p_vci(signal_vci_rom);

    // ram
    ram.p_clk(signal_clk);
    ram.p_resetn(signal_resetn);
    ram.p_vci(signal_vci_ram);

    // uart
    vcitty.p_clk(signal_clk);
    vcitty.p_resetn(signal_resetn);
    vcitty.p_vci(signal_vci_tty);
    vcitty.p_irq[0](signal_tty_irq);
    
    ////////////////////////////////////////////////////////////
    //////////////// Traces ////////////////////////////////////
    ////////////////////////////////////////////////////////////

#ifdef DO_TRACES
    sc_trace_file *TRACEFILE;
    TRACEFILE = sc_create_vcd_trace_file("vcd_traces");
    sc_trace (TRACEFILE, signal_resetn, "resetn" );
    
    sc_trace (TRACEFILE, signal_clk,    "100clk"    );
    sc_trace (TRACEFILE, signal25_clk,    "25clk"    );
    sc_trace(TRACEFILE, pixel_out, "pixel_out");
    sc_trace(TRACEFILE, line_out_valid, "line_out_valid");
    sc_trace(TRACEFILE, frame_out_valid, "frame_out_valid");

    sc_trace(TRACEFILE, pixel_in, "pixel_in");

    sc_trace(TRACEFILE, line_in_valid,"line_in_valid");
    sc_trace(TRACEFILE, frame_in_valid,"frame_in_valid");
    sc_trace(TRACEFILE, signal_wb_video_out_mastermodule,"video_out_master");
    sc_trace(TRACEFILE, start_loading_connection,"start_loading");
    
    
    /*
     
    sc_trace (TRACEFILE, signal_wb_ram, "ram_wb" );
    sc_trace (TRACEFILE, signal_wb_lm32,"lm32_wb");
    sc_trace (TRACEFILE, signal_vci_rom,"rom_vci");
    sc_trace (TRACEFILE, signal_wb_rom, "rom_wb" );
    sc_trace (TRACEFILE, signal_wb_tty, "tty_wb" );
    */
    sc_trace (TRACEFILE, signal_wb_mastermodule,"mastermoduleSIGNALS"  ); 
    sc_trace  (TRACEFILE, signal_wb_dummy_write,"dummywrite"  );
    sc_trace  (TRACEFILE, signal_wb_dummy,"dummyread"  );

#endif
       
    ////////////////////////////////////////////////////////////
    //////////////// Start Simulation //////////////////////////
    ////////////////////////////////////////////////////////////

    // unconnected irqs
    unconnected_irq = false;
    
       
    // reset
    sc_start(sc_core::sc_time(0, SC_NS));
     
    signal_resetn = false;
    sc_start(sc_core::sc_time(1, SC_NS));
    signal_resetn = true;

    // print maping table
    std::cout << "Let's go..." << std::endl;
    std::cout << maptab;

    // run SystemC simulator
    //     sc_start(sc_core::sc_time(1000, SC_NS));
    
    
    sc_start( );

#ifdef DO_TRACES
    sc_close_vcd_trace_file(TRACEFILE);
#endif

    return EXIT_SUCCESS;
}

// fake sc_man to catch exceptions
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

