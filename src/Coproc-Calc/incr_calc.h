#define TILE_HEIGHT 16
#define TILE_WIDTH 16

#include <systemc>

using namespace sc_core;
using namespace std;

namespace soclib { namespace caba {

    class IncrCalc
        :sc_module {

	private:
	    /*private line and column counter*/
	    int lin;
	    int col;

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
		sc_in<int>          p0_in;		
		sc_in<int>          q0_in;
		sc_in<int>          q1_in;
		sc_in<int>          q2_in;
		sc_in<int>          q3_in;
		sc_in<int>          r0_in;
		sc_in<int>          r1_in;
		sc_in<int>          r2_in;
		sc_in<int>          s0_in;
		sc_in<int>          s1_in;
		
		sc_signal<bool> wait_cyc; //wait one cycle for signal to be valid
		
		/* registres */

		int q0;  //constante	
		sc_signal<int> q1;

		sc_signal<int> q2;

		sc_signal<int> q3;


		int r0;  //constante
		sc_signal<int> r1;
		sc_signal<int> r2;


		int s0; //constante
		sc_signal<int> s1;

		int p0; //constante

		sc_signal<int> p1;

		sc_signal<int> p1_l;

		sc_signal<int> p2;

		sc_signal<int> p2_l;

		sc_signal<int> p3;

		sc_signal<int> p3_l;
			
		sc_out<bool> o_valid;
		sc_out<bool> o_finished;
		
                sc_out<int>   x_3;
                sc_out<int>   x_2;
                sc_out<int>   x_1;

		IncrCalc(sc_module_name insname);

            protected:
                SC_HAS_PROCESS(IncrCalc);

	};
    }
}
