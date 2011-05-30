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

#include "wb_simple_slave.h"
#include "wb_simple_master.h"

// locals
#include "segmentation.h"

//Video-gen / display
#include <video_gen.h>
#include <display.h>


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
    maptab.add(Segment("wb_slave", WBS_BASE, WBS_SIZE, IntTab(3), false));

    // Gloabal signals
    sc_time     clk_periode(10, SC_NS); // clk period
    sc_clock	signal_clk("signal_clk",clk_periode);
    sc_signal<bool> signal_resetn("signal_resetn");

    // interconnection signals
    soclib::caba::VciSignals<vci_param> signal_vci_tty("signal_vci_tty");
    soclib::caba::VciSignals<vci_param> signal_vci_rom("signal_vci_vcirom");
    soclib::caba::VciSignals<vci_param> signal_vci_ram("signal_vci_vciram");

    // WB interconnect signals
    soclib::caba::WbSignal<wb_param> signal_wb_lm32("signal_wb_lm32");
    soclib::caba::WbSignal<wb_param> signal_wb_ram("signal_wb_ram");
    soclib::caba::WbSignal<wb_param> signal_wb_rom("signal_wb_rom");
    soclib::caba::WbSignal<wb_param> signal_wb_tty("signal_wb_tty");
    soclib::caba::WbSignal<wb_param> signal_wb_slave("signal_wb_slave");
    soclib::caba::WbSignal<wb_param> signal_wb_master("signal_wb_master");

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

    // WB interconnect
    //                                           sc_name    maptab  masters slaves
    soclib::caba::WbInterco<wb_param> wbinterco("wbinterco",maptab, 2,4);

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

    soclib::caba::WbSimpleSlave<wb_param> simple_slave("WB_simple_slave");
    simple_slave.p_clk		(signal_clk);
    simple_slave.p_resetn	(signal_resetn);
    simple_slave.p_wb		(signal_wb_slave);

    soclib::caba::WbSimpleMaster<wb_param> simple_master("WB_simple_master");
    simple_master.p_clk		(signal_clk);
    simple_master.p_resetn	(signal_resetn);
    simple_master.p_wb		(signal_wb_master);

    ////////////////////////////////////////////////////////////
    ///////////////////// WB Net List //////////////////////////
    ////////////////////////////////////////////////////////////

    wbinterco.p_clk(signal_clk);
    wbinterco.p_resetn(signal_resetn);

    wbinterco.p_from_master[0](signal_wb_lm32);
    wbinterco.p_from_master[1](signal_wb_master);

    wbinterco.p_to_slave[0](signal_wb_rom);
    wbinterco.p_to_slave[1](signal_wb_ram);
    wbinterco.p_to_slave[2](signal_wb_tty);
    wbinterco.p_to_slave[3](signal_wb_slave);

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
    sc_trace (TRACEFILE, signal_clk,    "clk"    );
    sc_trace (TRACEFILE, signal_wb_lm32,"lm32_wb");
    sc_trace (TRACEFILE, signal_wb_ram, "ram_wb" );
    sc_trace (TRACEFILE, signal_vci_rom,"rom_vci");
    sc_trace (TRACEFILE, signal_wb_rom, "rom_wb" );
    sc_trace (TRACEFILE, signal_wb_tty, "tty_wb" );
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
    // sc_start(sc_core::sc_time(1000, SC_NS));
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

