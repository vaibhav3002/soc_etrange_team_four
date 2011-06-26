/************************************************************
 *
 *      File : incr_calc.cpp
 *      Author: A. Schindler
 *      Credits: T. Graba
 *      Telecom ParisTECH
 *
 *      Incremental calculation module
 *
 ************************************************************/


#include <cstdio>
#include "incr_calc.h"

#define tmpl(x) template<typename wb_param> x IncrCalc<wb_param>

namespace soclib { namespace caba {

   // le constructeur 
   tmpl(/**/)::IncrCalc(sc_core::sc_module_name insname):
      sc_core::sc_module(insname), reg("IncrCalcReg")
   {
     // X and Y calculations run in parallel
      SC_THREAD(calc_x);
      sensitive << clk.pos();
      SC_THREAD(calc_y);
      sensitive << clk.pos();
   }

    tmpl(void)::calc_x()
    {
      while(1)
	{
	  if(reset_n == false)
	    {
	      cout << "RESET !!" << endl;
	      /*reinitialise line and column count to zero*/
	      lin_x = 0;
	      col_x = 0;
	      state_x = 0;
	      //sets outputs low
	      o_valid = 0;
	      o_finished = 0;

	      wait();
	    }
	  else
	    {
	      /* if en is not high, the module passes one clock cycle without any changes to the O/P signals */
	      if(en)
		{
		  /* state machine */
		  switch(state_x)
		    {
		      /* wait state */
		    case 0 : {
		      lin_x = 0;
		      col_x = 0;
		      o_valid = 0;
		      o_finished = 0;
		      if(start)
			{
			  /* load coefficients */
			  state_x = 1;
			  p0_x = (mfixed)reg.slave_read(0);
			  q0_x = (mfixed)reg.slave_read(24);
			  q1_x = (mfixed)reg.slave_read(28);
			  q2_x = (mfixed)reg.slave_read(32);
			  q3_x = (mfixed)reg.slave_read(36);
			  r0_x = (mfixed)reg.slave_read(12);
			  r1_x = (mfixed)reg.slave_read(16);
			  r2_x = (mfixed)reg.slave_read(20);
			  s0_x = (mfixed)reg.slave_read(4);
			  s1_x = (mfixed)reg.slave_read(8);
			}
		      /* extract digit and fractional part */
		      x = getH(p3_x);
		      dx = getL(p3_x);
		      y = getH(p3_x);
		      dy = getL(p3_x);
		      break;
		    }
		    case 1: {
		      /* calculation state */
		      if(!start)
			{
			  /* output is valid in this state */
			  o_valid = 1;
			  /* extract digit and fractional part and output */
			  x = getH(p3_x);
			  dx = getL(p3_x);
			  y = getH(p3_x);
			  dy = getL(p3_x);

			  /* for the first column of each line, load directly from qx */
			  if(col_x == 0) /* if beginning of new line, must do +Y formulas */
			    {
			      p3_x = q3_x;
			      p2_x = r2_x;
			      p1_x = s1_x;

			      /*incremental calculation */
			      q3_x = q3_x + q2_x;

			      q2_x = q2_x + q1_x;
			      q1_x = q1_x + q0_x;

			      r2_x = r2_x + r1_x;
			      r1_x = r1_x + r0_x;

			      s1_x = s1_x + s0_x;
			    }
			  else
			    {
			      /* else, use the other formulas, i.e. +X */
			      p3_x = p3_x + p2_x;
			      p2_x = p2_x + p1_x;
			      p1_x = p1_x + p0_x;
			    }
			  col_x++;
			  if (col_x == TILE_WIDTH)
			    {
			      col_x = 0;
			      cout << "COPROC : New line " << endl;
			      lin_x++;
			      if(lin_x == TILE_HEIGHT)
				{
				  state_x=2; //go to wait state, if finished the tile
				}
			    }
			}

		      break;
		    }
		    case 2: {
		      /* stays there for one state to output finished high */
		      o_finished = 1;
		      state_x = 0;
		      o_valid = 0;
		      break;
		    }break;
		    }
		}
	      wait();
	    }
	}
    }

    tmpl(void)::calc_y()
	       /* clone of calc_x */
    {
       while(1)
	{
	  if(reset_n == false)
	    {
	      cout << "RESET !!" << endl;
	      ///reinitialise line and column count to zero
	      lin_y = 0;
	      col_y = 0;
	      state_y = 0;

	      o_valid = 0;
	      o_finished = 0;


	      wait();
	    }
	  else
	    {
	      if(en)
		{
		  switch(state)
		    {
		    case 0 : {
		      lin_y = 0;
		      col_y = 0;
		      o_valid = 0;
		      o_finished = 0;
		      if(start_y)
			{
			  state = 1;
			  p0_y = (mfixed)reg.slave_read(40+0);
			  q0_y = (mfixed)reg.slave_read(40+24);
			  q1_y = (mfixed)reg.slave_read(40+28);
			  q2_y = (mfixed)reg.slave_read(40+32);
			  q3_y = (mfixed)reg.slave_read(40+36);
			  r0_y = (mfixed)reg.slave_read(40+12);
			  r1_y = (mfixed)reg.slave_read(40+16);
			  r2_y = (mfixed)reg.slave_read(40+20);
			  s0_y = (mfixed)reg.slave_read(40+4);
			  s1_y = (mfixed)reg.slave_read(40+8);
			}
		      y = getH(p3_y);
		      dy = getL(p3_y);
		      break;
		    }
		    case 1: {
		      if(!start)
			{
			  o_valid = 1;

			  y = getH(p3_y);
			  dy = getL(p3_y);

			  cout << "Coproc " << lin_y << " " << col_y << endl;

			  if(col_y == 0)
			    {
			      p3_y = q3_y;
			      p2_y = r2_y;
			      p1_y = s1_y;

			      q3_y = q3_y + q2_y;

			      q2_y = q2_y + q1_y;
			      q1_y = q1_y + q0_y;

			      r2_y = r2_y + r1_y;
			      r1_y = r1_y + r0_y;

			      s1_y = s1_y + s0_y;
			    }
			  else
			    {
			      p3_y = p3_y + p2_y;
			      p2_y = p2_y + p1_y;
			      p1_y = p1_y + p0_y;
			    }

			}
			  else
			    {
			      p3_y = p3_y + p2_y;
			      p2_y = p2_y + p1_y;
			      p1_y = p1_y + p0_y;
			    }
			  col_y++;
			  if (col_y == TILE_WIDTH)
			    {
			      col_y = 0;
			      cout << "COPROC : New line " << endl;
			      lin_y++;
			      if(lin_y == TILE_HEIGHT)
				{
				  state_y = 2; //go to wait state
				}
			    }
			}

		      break;
		    }
		    case 2: {
		      o_finished = 1;
		      state_y = 0;
		      o_valid = 0;
		      break;
		    }break;
		    }
		}
	      wait();
	    }
	}
    }
	      
  }
}
