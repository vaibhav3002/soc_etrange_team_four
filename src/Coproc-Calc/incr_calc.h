#ifndef INCR_CALC
#define INCR_CALC

#define TILE_HEIGHT 16
#define TILE_WIDTH 16

#include <systemc.h>
#include "mfixed2.h"
#include "wb_slave_reg_coproc.h"

using namespace sc_core;
using namespace std;

namespace soclib { namespace caba {

   template<typename wb_param>
      class IncrCalc : sc_core::sc_module {

         private:
            /*private line and column counter*/
            int lin;
            int col;
            //int tile;

            int state;
            /* 0 : WAIT
               1 : Calculate
               2 : Finished
            */

            /*methods*/
            void calc();

         public:
            // IO PORTS

            sc_in_clk           clk;
            sc_in<bool>         reset_n;

            sc_in<bool>         start;
            sc_in<bool>         en;
            sc_in<mfixed>          p0_in;		
            sc_in<mfixed>          q0_in;
            sc_in<mfixed>          q1_in;
            sc_in<mfixed>          q2_in;
            sc_in<mfixed>          q3_in;
            sc_in<mfixed>          r0_in;
            sc_in<mfixed>          r1_in;
            sc_in<mfixed>          r2_in;
            sc_in<mfixed>          s0_in;
            sc_in<mfixed>          s1_in;

            sc_signal<bool> wait_cyc; //wait one cycle for signal to be valid

            /* registres */

            mfixed q0;  //constante	
            sc_signal<mfixed> q1;

            sc_signal<mfixed> q2;

            sc_signal<mfixed> q3;

            WbSlaveRegModuleCoproc<wb_param> reg;
            mfixed r0;  //constante
            sc_signal<mfixed> r1;
            sc_signal<mfixed> r2;


            mfixed s0; //constante
            sc_signal<mfixed> s1;

            mfixed p0; //constante

            sc_signal<mfixed> p1;

            sc_signal<mfixed> p1_l;

            sc_signal<mfixed> p2;

            sc_signal<mfixed> p2_l;

            sc_signal<mfixed> p3;

            sc_signal<mfixed> p3_l;

            sc_out<bool> o_valid;
            sc_out<bool> o_finished;

            /*sc_out<mfixed>   x_3;
              sc_out<mfixed>   x_2;
              sc_out<mfixed>   x_1;*/

            sc_out<unsigned short> x;
            sc_out<short> dx;

            IncrCalc(sc_module_name insname);

         protected:
            SC_HAS_PROCESS(IncrCalc);

      };
}
}

#endif
