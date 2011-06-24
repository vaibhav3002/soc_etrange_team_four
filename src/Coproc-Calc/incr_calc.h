#define TILE_HEIGHT 16
#define TILE_WIDTH 16
#define IMAGE_HEIGHT 480
#define IMAGE_WIDTH 640
#define NO_TILES 1200

#include "systemc.h"
#include "mfixed2.h"

using namespace sc_core;
using namespace std;

namespace soclib { namespace caba {

    class IncrCalc
        :sc_module {

	private:
	    /*private line and column counter*/
	    int lin;
	    int col;
	    int tile;

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
		sc_in<bool>         p0_valid;
		sc_in<bool>         q0_valid;
		sc_in<bool>         q1_valid;
		sc_in<bool>         q2_valid;
		sc_in<bool>         r0_valid;
		sc_in<bool>         r1_valid;
		sc_in<bool>         s0_valid;
		sc_in<bool>         load;
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
		
                sc_out<mfixed>   x_3;
                sc_out<mfixed>   x_2;
                sc_out<mfixed>   x_1;

		IncrCalc(sc_module_name insname);

            protected:
                SC_HAS_PROCESS(IncrCalc);

	};
    }
}
