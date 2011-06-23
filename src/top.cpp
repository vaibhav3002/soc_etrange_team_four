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

#define DO_TRACES 1  

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

//include slave register fro video_in video_out modules
#include "wb_slave_reg.h"

//include interpolator vector generator
#include "interpolation_test_module.h"

//include interpolator
#include "interpolator.h"

//include interpolator_out module
#include "interpolator_out.h"

//include buffer management module
#include "buffer_management.h"

//include buffer management test module
#include "buffer_management_vector_generator.h"
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
    maptab.add(Segment("Video_in_ram_target",VIDEO_IN_REG,1,IntTab(3),false));    
    maptab.add(Segment("Video_out_ram_source",VIDEO_OUT_REG,1,IntTab(4),false));    
//    maptab.add(Segment("Interpolator out reg",INTERPOLATOR_OUT_REG,1,IntTab(5),false));    
//    maptab.add(Segment("Buffer management reg",BUFFER_MANAGEMENT_REG,1,IntTab(6),false));    
  
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

   //video_gen to master signals 
   sc_time                 clk25_periode(40, SC_NS);
   sc_clock                signal25_clk("signal25_clk",clk25_periode);        //25mhz clock piwel clock

   //interpolator test signals
   sc_signal<bool>                 signal_pixel_valid;
   sc_signal<unsigned char>        signal_test_pixel_out;
   sc_signal<unsigned char>        signal_dx;
   sc_signal<unsigned char>        signal_dy;

   //interpolator signals

   sc_signal<unsigned char>     signal_interpolator_pixel_out;
   sc_signal<bool>              signal_interpolator_pixel_valid;
   sc_signal<bool>              signal_interpolator_pixel_out_valid;
   
   //buffer management signals
   //inpt connections
   sc_signal<bool>     signal_buffer_management_pixel_in_valid;
   sc_signal<uint16_t>     signal_buffer_management_dx_in;
   sc_signal<uint16_t>     signal_buffer_management_dy_in;
   sc_signal<unsigned char>     signal_buffer_management_x;
   sc_signal<unsigned char>     signal_buffer_management_y;
   //output conncetions
   sc_signal<unsigned char>     signal_buffer_management_pixel_out;
   sc_signal<bool>     signal_buffer_management_tile_ready;
   sc_signal<bool>     signal_buffer_management_pixel_out_valid;
   sc_signal<unsigned char>     signal_buffer_management_dx_out;
   sc_signal<unsigned char>     signal_buffer_management_dy_out;
	
   //buffer_management test module 
   //output connections 
    sc_signal<bool> signal_buffer_management_test_addr_valid;
    
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
  //  soclib::caba::WbSignal<wb_param> signal_wb_dummy("wb_dummy"); 
  //  soclib::caba::WbSignal<wb_param> signal_wb_dummy_write("wb_dummy_write"); 
    soclib::caba::WbSignal<wb_param> signal_wb_video_in_reg("signal_wb_video_in_reg");
    soclib::caba::WbSignal<wb_param> signal_wb_video_out_reg("signal_wb_video_out_reg");
    soclib::caba::WbSignal<wb_param> signal_wb_interpolator_out("signal_wb_interpolator_out");
    soclib::caba::WbSignal<wb_param> signal_wb_buffer_management("signal_wb_buffer_management_out");
    soclib::caba::WbSignal<wb_param> signal_wb_buffer_management_reg("signal_wb_buffer_management_out_reg ");
    
    soclib::caba::WbSignal<wb_param> signal_wb_interpolator_out_reg("signal_wb_video_out_reg");
    // irq from uart
    sc_signal<bool> signal_tty_irq("signal_tty_irq");
    // irq from video_in
    sc_signal<bool> signal_videoin_irq("signal_videoin_irq");
    // irq from video_out
    sc_signal<bool> signal_videoout_irq("signal_videoout_irq");
    //irq from interpolator_out 
    sc_signal<bool> signal_interpolator_out_irq("signal_videoout_irq");
    // unconnected irqs
    sc_signal<bool> unconnected_irq ("unconnected_irq");
	//buffer management irq out
   sc_signal<bool>     signal_buffer_management_irq_out;

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
    //5 Masters: video_in, video_out, processor, interpolator_out,buffer management
    //7 slaves, ram, rom, tty, video_in_ram target register, video_out_source register,interpolator out,buffer management
    soclib::caba::WbInterco<wb_param> wbinterco("wbinterco",maptab, 5,7);

    //Video Gen creation and instantiation
    
    soclib::caba::VideoGen  videogen("video_gen");     
    videogen.clk(signal25_clk);
    videogen.line_valid (line_in_valid); 
    videogen.frame_valid(frame_in_valid);
    videogen.reset_n(signal_resetn);
    videogen.pixel_out(pixel_in);
    
    //Video_in Master generation and instantiation
    
    soclib::caba::Video_in<wb_param>    video_in_master_module  ("video_in");
    video_in_master_module.p_clk(signal25_clk); 
    video_in_master_module.p_clk_100mhz(signal_clk);
    video_in_master_module.p_resetn(signal_resetn);
    video_in_master_module.line_valid(line_in_valid);
    video_in_master_module.frame_valid(frame_in_valid);
    video_in_master_module.pixel_in(pixel_in);
    video_in_master_module.p_wb(signal_wb_mastermodule);
    video_in_master_module.irq_out(signal_videoin_irq);
    video_in_master_module.reg0.p_clk(signal_clk);
    video_in_master_module.reg0.p_wb(signal_wb_video_in_reg);
    video_in_master_module.reg0.p_resetn(signal_resetn);
   
    //Video_out Master generation and instantiation
	
   soclib::caba::Video_out_Master <wb_param>    video_out_master_module  ("video_out_master");
	video_out_master_module.p_clk_100mhz(signal_clk );
	video_out_master_module.p_clk(signal25_clk);
	video_out_master_module.p_resetn(signal_resetn);
	video_out_master_module.line_valid(line_out_valid);
	video_out_master_module.frame_valid(frame_out_valid);
	video_out_master_module.pixel_out(pixel_out);
	video_out_master_module.p_wb(signal_wb_video_out_mastermodule);
	video_out_master_module.reg0.p_clk(signal_clk);
	video_out_master_module.reg0.p_resetn(signal_resetn);
	video_out_master_module.reg0.p_wb(signal_wb_video_out_reg);
	video_out_master_module.irq_out(signal_videoout_irq);


   //display instantiation	
   soclib::caba::Display display("display"); 
	display.clk(signal25_clk);
	display.reset_n(signal_resetn);
	display.line_valid(line_out_valid);
	display.frame_valid(frame_out_valid);
	display.pixel_in(pixel_out);

/*	
   //dummy read instantiation
	soclib::caba::Master_dummy <wb_param>    master_dummy  ("master_dummy_read");
	master_dummy.p_clk_100mhz(signal_clk);
	master_dummy.p_resetn(signal_resetn);
	master_dummy.p_wb(signal_wb_dummy);

	//dummy write instantiation
	soclib::caba::Master_dummy_write <wb_param>    master_dummy_write  ("master_dummy_write");
	master_dummy_write.p_clk_100mhz(signal_clk);
	master_dummy_write.p_resetn(signal_resetn);
	master_dummy_write.p_wb(signal_wb_dummy_write);
*/
//interpolator module instatiation

   soclib::caba::testinterpolate testinterpolate("interpolator_vector"); 
  	
	testinterpolate.p_clk_100mhz(signal_clk); 
	testinterpolate.p_resetn(signal_resetn); 
	testinterpolate.dx(signal_dx); 
	testinterpolate.dy(signal_dy); 
	testinterpolate.pixel_valid(signal_pixel_valid); 
	testinterpolate.pixel_out(signal_test_pixel_out); 

//interpolator instanciation

   soclib::caba::interpolator interpolator("interpolator"); 
	interpolator.p_clk(signal_clk);
	interpolator.p_resetn(signal_resetn);
	interpolator.dx(signal_buffer_management_dx_out);
	interpolator.dy(signal_buffer_management_dy_out);
	interpolator.pixel_in(signal_buffer_management_pixel_out);
	interpolator.pixel_valid(signal_buffer_management_pixel_out_valid);
	interpolator.pixel_out_valid(signal_interpolator_pixel_valid);
	interpolator.pixel_out(signal_interpolator_pixel_out);
//interpolator out instanciation

   soclib::caba::Interpolator_out<wb_param> interpolator_out("interpolator_out"); 
   interpolator_out.p_clk_100mhz(signal_clk); 
   interpolator_out.p_resetn(signal_resetn); 
   interpolator_out.pixel_valid(signal_interpolator_pixel_valid); 
   interpolator_out.pixel_in(signal_interpolator_pixel_out); 
   interpolator_out.p_wb(signal_wb_interpolator_out);
   interpolator_out.reg0.p_clk(signal_clk);
   interpolator_out.reg0.p_resetn(signal_resetn);
   interpolator_out.reg0.p_wb(signal_wb_interpolator_out_reg);
   interpolator_out.irq_out(signal_interpolator_out_irq);
   
//Buffer management instanciation
 soclib::caba::Buffer_management<wb_param> buffer_management("buffer_management");
	//inputs
	buffer_management.p_clk_100mhz(signal_clk);
	buffer_management.p_resetn(signal_resetn);
	buffer_management.pixel_in_valid(signal_buffer_management_pixel_in_valid);
	buffer_management.dx_in(signal_buffer_management_dx_in);
	buffer_management.dy_in(signal_buffer_management_dy_in);
	buffer_management.x(signal_buffer_management_x);
	buffer_management.y(signal_buffer_management_y);
	//outputs
	buffer_management.pixel_out(signal_buffer_management_pixel_out);
	buffer_management.tile_ready(signal_buffer_management_tile_ready);
	buffer_management.pixel_out_valid(signal_buffer_management_pixel_out_valid);
	buffer_management.dx_out(signal_buffer_management_dx_out);
	buffer_management.dy_out(signal_buffer_management_dy_out);
	buffer_management.irq_out(signal_buffer_management_irq_out);
        buffer_management.p_wb(signal_wb_buffer_management);
        buffer_management.reg0.p_clk(signal_clk);
        buffer_management.reg0.p_resetn(signal_resetn);
        buffer_management.reg0.p_wb(signal_wb_buffer_management_reg);
//Buffer management vector generator
soclib::caba::testbuffermanagement test_buffer_management("test_buffer_management");
        test_buffer_management.p_resetn(signal_resetn);
        test_buffer_management.p_clk_100mhz(signal_clk);
	test_buffer_management.pixel_valid(signal_buffer_management_pixel_in_valid);
	test_buffer_management.dx(signal_buffer_management_dx_in);
	test_buffer_management.dy(signal_buffer_management_dy_in);
	test_buffer_management.x(signal_buffer_management_x);
	test_buffer_management.y(signal_buffer_management_y);
//test register instantiation
 //  soclib::caba::WbSlaveModule <wb_param> test_slave ("test_slave");
 //   test_slave.p_clk(signal_clk);
 //   test_slave.p_wb(signal_wb_slave);
 //   test_slave.p_resetn(signal_resetn);

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
    wbinterco.p_to_slave[3](signal_wb_video_in_reg);
    wbinterco.p_to_slave[4](signal_wb_video_out_reg);
    wbinterco.p_to_slave[5](signal_wb_interpolator_out_reg);
    wbinterco.p_to_slave[6](signal_wb_buffer_management_reg);
    wbinterco.p_from_master[1](signal_wb_mastermodule);
    wbinterco.p_from_master[2](signal_wb_video_out_mastermodule);
    wbinterco.p_from_master[3](signal_wb_interpolator_out);
    wbinterco.p_from_master[4](signal_wb_buffer_management);

    // lm32
    lm32.p_clk(signal_clk);
    lm32.p_resetn(signal_resetn);
    lm32.p_wb(signal_wb_lm32);
    // !! the real LM32 irq are active low
    // To avoid adding inverters here, we consider
    // them active high
    lm32.p_irq[0] (signal_tty_irq);
    lm32.p_irq[1] (signal_videoin_irq);
    lm32.p_irq[2] (signal_videoout_irq);
    lm32.p_irq[3] (signal_buffer_management_irq_out);
    for (int i=4; i<32; i++)
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
    /*
    sc_trace(TRACEFILE, pixel_out, "pixel_out");
    sc_trace(TRACEFILE, line_out_valid, "line_out_valid");
    sc_trace(TRACEFILE, frame_out_valid, "frame_out_valid");

    sc_trace(TRACEFILE, pixel_in, "pixel_in");

    sc_trace(TRACEFILE, line_in_valid,"line_in_valid");
    sc_trace(TRACEFILE, frame_in_valid,"frame_in_valid");
    sc_trace(TRACEFILE, signal_wb_video_out_mastermodule,"video_out_master");

    sc_trace(TRACEFILE, signal_videoin_irq, "video_in_irq");

    sc_trace(TRACEFILE, signal_videoout_irq, "video_out_irq");
    */
   
    sc_trace(TRACEFILE,signal_wb_interpolator_out, "signal_wb_interpol_out");
    sc_trace(TRACEFILE,signal_interpolator_pixel_out, "signal_interpolator_pixel_out");
    sc_trace(TRACEFILE,signal_interpolator_pixel_valid, "signal_pixel_out_interpolate_valid");
    sc_trace(TRACEFILE,signal_interpolator_pixel_out, "signal_pixel_out_interpolate");
    sc_trace(TRACEFILE,signal_buffer_management_dx_out, "buffer_management_dx_out");
    sc_trace(TRACEFILE,signal_buffer_management_dy_out, "buffer_management_dy_out");
    sc_trace(TRACEFILE,signal_buffer_management_dx_in, "buffer_management_dx_in");
    sc_trace(TRACEFILE,signal_buffer_management_dy_in, "buffer_management_dy_in");
     sc_trace(TRACEFILE,signal_buffer_management_x, "buffer_management_x_in");
     sc_trace(TRACEFILE,signal_buffer_management_y, "buffer_management_y_in");
     sc_trace(TRACEFILE,signal_buffer_management_pixel_out, "buffer_management_pixel_out");
     sc_trace(TRACEFILE,signal_buffer_management_pixel_out_valid, "buffer_management_pixel_out_valid");
     sc_trace(TRACEFILE,signal_buffer_management_pixel_in_valid, "buffer_management_pixel_in_valid");
    /*
     
    sc_trace (TRACEFILE, signal_wb_ram, "ram_wb" );
    sc_trace (TRACEFILE, signal_wb_lm32,"lm32_wb");
    sc_trace (TRACEFILE, signal_vci_rom,"rom_vci");
    sc_trace (TRACEFILE, signal_wb_rom, "rom_wb" );
    sc_trace (TRACEFILE, signal_wb_tty, "tty_wb" );
    
    sc_trace (TRACEFILE, signal_wb_mastermodule,"video_in_master"  ); 
    sc_trace  (TRACEFILE, signal_wb_dummy_write,"dummywrite"  );
    sc_trace  (TRACEFILE, signal_wb_dummy,"dummyread"  );
    sc_trace  (TRACEFILE, signal_wb_video_in_reg,"signal_wb_slave");
	*/
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

    // print mapping table
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

