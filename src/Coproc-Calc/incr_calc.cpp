#include <cstdio>
#include "incr_calc.h"

#define tmpl(x) template<typename wb_param> x IncrCalc<wb_param>

namespace soclib { namespace caba {

   // le constructeur 
   tmpl(/**/)::IncrCalc(sc_core::sc_module_name insname):
      sc_core::sc_module(insname), reg("IncrCalcReg")
   {
      
      SC_THREAD(calc);
      sensitive << clk.pos();
   }

   tmpl(void)::calc()
   {
      while(1)
      {
         if(reset_n == false)
            /*resets always to x=0, y=0*/
         {
            cout << "RESET !!" << endl;
            /*reinitialise line and column count to zero*/
            lin = 0;
            col = 0;
            state = 0;
            //tile = 0;

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
                              lin = 0;
                              col = 0;
                              o_valid = 0;
                              o_finished = 0;
                              if(start)
                              {
                                 state = 1;
                                 p0 = (mfixed)reg.slave_read(0);
                                 q0 = (mfixed)reg.slave_read(24);
                                 q1 = (mfixed)reg.slave_read(28);
                                 q2 = (mfixed)reg.slave_read(32);
                                 q3 = (mfixed)reg.slave_read(36);
                                 r0 = (mfixed)reg.slave_read(12);
                                 r1 = (mfixed)reg.slave_read(16);
                                 r2 = (mfixed)reg.slave_read(20);
                                 s0 = (mfixed)reg.slave_read(4);
                                 s1 = (mfixed)reg.slave_read(8);
                              }
                              x = getH(p3);
                              dx = getL(p3);
                              break;
                           }
                  case 1: {
                             if(!start)
                             {
                                o_valid = 1;
                                //x_3 = p3;
                                x = getH(p3);
                                dx = getL(p3);
                                //x_2 = p2;
                                //x_1 = p1;

                                cout << "Coproc " << lin << " " << col << endl;

                                if(col == 0)
                                {
                                   p3 = q3;
                                   p2 = r2;
                                   p1 = s1;

                                   q3 = q3 + q2;
                                }
                                else
                                {
                                   p3 = p3 + p2;
                                   p2 = p2 + p1;
                                   p1 = p1 + p0;
                                }
                                col++;
                                if (col == TILE_WIDTH)
                                {
                                   col = 0;
                                   cout << "COPROC : New line " << endl;
                                   lin++;
                                   if(lin == TILE_HEIGHT)
                                   {
                                      state=2; //go to wait state
                                   }
                                }
                             }

                             break;
                          }
                  case 2: {
                             o_finished = 1;
                             state = 0;
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
