/************************************************************
 *
 *      File : incr_calc.h
 *      Author: A. Schindler
 *      Credits: T. Graba
 *      Telecom ParisTECH
 *
 *      Incremental calculation module
 *
 ************************************************************/

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
      /*private line and column counter for x and y */
      /* should be the same values, but need separate counters since the
	 processes run in parallel */
      int lin_x;
      int col_x;

      int lin_y;
      int col_y;

      int state_x;
      int state_y;
      /* 0 : WAIT
	 1 : Calculate
	 2 : Finished
      */

      /*methods*/
      void calc_x();
      void calc_y();

    public:
      // IO PORTS

      sc_in_clk           clk;
      sc_in<bool>         reset_n;

      sc_in<bool>         start;
      sc_in<bool>         en;

      /* registres */

      WbSlaveRegModuleCoproc<wb_param> reg;

      /* outputs */

      sc_out<bool> o_valid;
      sc_out<bool> o_finished;

      sc_out<unsigned char> x;
      sc_out<unsigned short> dx;
      sc_out<unsigned char> y;
      sc_out<unsigned short> dy;

      /* internal registers for the x coefficients  */

      mfixed r0_x;  //constante
      sc_signal<mfixed> r1_x;
      sc_signal<mfixed> r2_x;


      mfixed s0_x; //constante
      sc_signal<mfixed> s1_x;

      mfixed p0_x; //constante

      sc_signal<mfixed> p1_x;

      sc_signal<mfixed> p1_l_x;

      sc_signal<mfixed> p2_x;

      sc_signal<mfixed> p2_l_x;

      sc_signal<mfixed> p3_x;

      sc_signal<mfixed> p3_l_x;

      mfixed q0_x;  //constante	
      sc_signal<mfixed> q1_x;

      sc_signal<mfixed> q2_x;

      sc_signal<mfixed> q3_x;

         /* internal registers for the y coefficients  */

      mfixed r0_y;  //constante
      sc_signal<mfixed> r1_y;
      sc_signal<mfixed> r2_y;


      mfixed s0_y; //constante
      sc_signal<mfixed> s1_y;

      mfixed p0_y; //constante

      sc_signal<mfixed> p1_y;

      sc_signal<mfixed> p1_l_y;

      sc_signal<mfixed> p2_y;

      sc_signal<mfixed> p2_l_y;

      sc_signal<mfixed> p3_y;

      sc_signal<mfixed> p3_l_y;

      mfixed q0_y;  //constante	
      sc_signal<mfixed> q1_y;

      sc_signal<mfixed> q2_y;

      sc_signal<mfixed> q3_y;

      IncrCalc(sc_module_name insname);

    protected:
      SC_HAS_PROCESS(IncrCalc);

    };
  }
}

#endif
