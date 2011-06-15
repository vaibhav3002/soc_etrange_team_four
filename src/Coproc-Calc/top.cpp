#include "systemc.h"
#include "incr_calc.h"
#include <cmath>

void next_cycle (sc_signal<bool> &signal_clk);
void calc_coeffs (int i, int j);

    int a30 = 2;
    int a20 = -3;
    int a10 = 2;
    int a00 = 0;
    int a21 = 0;
    int a11 = -2;
    int a01 = 1;
    int a12 = 2;
    int a02 = -1;
    int a03 = 0;

    sc_signal<int> p0;
    sc_signal<int> p1;
    sc_signal<int> p2;
    sc_signal<int> p3;
    sc_signal<int> q0;
    sc_signal<int> q1;
    sc_signal<int> q2;
    sc_signal<int> q3;
    sc_signal<int> r0;
    sc_signal<int> r1;
    sc_signal<int> r2;
    sc_signal<int> s0;
    sc_signal<int> s1;

int sc_main(int argc, char *argv[])
{
    using namespace sc_core;
    using namespace soclib::caba;    

    int i, j;
    int x, y;
    int x_exp;

    sc_signal<bool> clk;
    sc_signal<bool> reset_n;
    sc_signal<int> x_3;
    sc_signal<int> x_2;
    sc_signal<int> x_1;
    sc_signal<bool> x_valid;
    sc_signal<bool> p0_valid;    
    sc_signal<bool> q0_valid;
    sc_signal<bool> q1_valid;
    sc_signal<bool> q2_valid;
    sc_signal<bool> q3_valid;
    sc_signal<bool> r0_valid;
    sc_signal<bool> r1_valid;
    sc_signal<bool> r2_valid;
    sc_signal<bool> s0_valid;
    sc_signal<bool> s1_valid;
    sc_signal<bool> load;
    sc_signal<bool> finished;



    sc_trace_file *my_trace_file;
    my_trace_file =
        sc_create_vcd_trace_file ("incr_calc_trace");
    sc_trace(my_trace_file, clk, "clk");
    sc_trace(my_trace_file, reset_n, "reset");
    sc_trace(my_trace_file, x_3, "x_3");
    sc_trace(my_trace_file, x_2, "x_2");
    sc_trace(my_trace_file, x_1, "x_1");
    sc_trace(my_trace_file, x_valid, "valid");
    sc_trace(my_trace_file, finished, "finished");

    IncrCalc my_calc("coproc_incr");
    my_calc.clk(clk);
    my_calc.reset_n(reset_n);
    my_calc.x_3(x_3);
    my_calc.x_2(x_2);
    my_calc.x_1(x_1);
    my_calc.p0_valid(p0_valid);
    my_calc.q0_valid(q0_valid);
    my_calc.q1_valid(q1_valid);
    my_calc.q2_valid(q2_valid);
    my_calc.r0_valid(r0_valid);
    my_calc.r1_valid(r1_valid);
    my_calc.s0_valid(s0_valid);
    my_calc.p0_in(p0);
    my_calc.q0_in(q0);
    my_calc.q1_in(q1);
    my_calc.q2_in(q2);
    my_calc.q3_in(q3);
    my_calc.r0_in(r0);
    my_calc.r1_in(r1);
    my_calc.r2_in(r2);
    my_calc.s0_in(s0);
    my_calc.s1_in(s1);
    my_calc.load(load);
    my_calc.o_finished(finished);

    my_calc.o_valid(x_valid);

    next_cycle(clk);
    next_cycle(clk);
    reset_n = true;
    next_cycle(clk);
    next_cycle(clk);
    next_cycle(clk);
    reset_n = false;
    next_cycle(clk);
    next_cycle(clk);
    next_cycle(clk);
    reset_n = true;

    x = 0;
    y = 0;
    x_exp = 0;

    //for each tile
    for(j=0; j<1; j++) //30 vertically
	for(i=0; i<3; i++) //40 horizontally
	{
	    x = TILE_WIDTH * i;
	    y = TILE_HEIGHT * j;
	    cout << "x : " << x << " y: " << y << endl;
	    calc_coeffs(i, j);
	    p0_valid = 1;
	    q0_valid = 1;
	    q1_valid = 1;
	    q2_valid = 1;
	    q3_valid = 1;
	    r0_valid = 1;
	    r1_valid = 1;
	    r2_valid = 1;
	    s0_valid = 1;
	    s1_valid = 1;
	    load = 1;
	    next_cycle(clk);
	    p0_valid = 0;
	    q0_valid = 0;
	    q1_valid = 0;
	    q2_valid = 0;
	    q3_valid = 0;
	    r0_valid = 0;
	    r1_valid = 0;
	    r2_valid = 0;
	    s0_valid = 0;
	    s1_valid = 0;
	    load = 0;
	    x_exp = 0;
	    next_cycle(clk);
	    while(x_valid && !finished)
	    {
		x_exp = a30 * pow(x,3) + a21 * pow(x,2) * y + a12 * x * pow(y,2) + a03 * pow(y,3) + a20 * pow(x,2) + a11 * x * y + a02 * pow(y,2) + a10 * x + a01 * y + a00;
		cout << "x : " << x << " y: " << y << endl;
		next_cycle(clk);
		cout << (int)x_exp << " " << (int)x_3 << endl;
		if(x_3 != x_exp)
		    cout << "ERROR" << endl;
		x++;
		if (x==16*i+TILE_WIDTH)
		{
		    x = 16 * i;
		    y++;    
		}
	    }
	    cout << "--------- FINISHED TILE " << i << " " << j << endl;
	}

    sc_close_vcd_trace_file (my_trace_file);

    return EXIT_SUCCESS;

}

void next_cycle (sc_signal<bool> &signal_clk)
{
    signal_clk = false;
    sc_start (1, SC_NS);
    signal_clk = true;
    sc_start (1, SC_NS);
}

void calc_coeffs (int i, int j)
{
    cout << "calc i " <<  i << " j " << j << endl;
    s0 = 2*a21;
    r0 = 2*a12;
    q0 = 6*a03;
    p0 = 6*a30;
    
    /*      c00          + 16*i*c10       + 16*j*c01         + s0 * j */
    s1 = (6*a30 + 2*a20) + 6*a30 * 16 * i + 2 * a21 * 16 * j;// + 2*a21 * 16 * j;

    /*    r1                                           +   r0 * 16 j  */
    r1 = 2*a12 * 16*i + 2*a12 * 16*j + a12 + a21 + a11 + 2*a12 * 16*j;

    r2 = pow((16*i),2) * 3*a30 + 2*a21*16*j*16*i + a12 * pow((16*j),2) + (3*a30+2*a20)*16*i + (a21+a11) * 16 * j + (a30 + a20 + a10); //+ /*16*j*r1*/ (16*j) * (2*a12 * 16*i + 2*a12 * 16*j + a12 + a21 + a11 + 2*a12 * 16*j);

    q1 = 2*a12 * 16*i + 6*a03 * 16*j + 6*a03 + 2*a02 + 6*a03 * 16*j;

    q2 = a21 * pow((16*i),2) + 2*a12*(16*i)*(16*j) + 3*a03*pow((16*j),2) + (a12 + a11)*(16*i) + (3*a03 + 2*a02) * (16*j) + (a03 + a02 + a01) + /*16*j * q1 */ (16 * j) * (2*a12 * 16*i + 6*a03 * 16*j + 6*a03 + 2*a02 + 6*a03 * 16*j);

    q3 = a30 * pow((16*i),3) + a21 * pow((16*i),2) * 16 * j + a12 * 16 * i * pow((16*j),2) + a03*pow((16*j),3) + a20*pow((16*i),2) + a11*16*i*16*j + a02*pow((16*j),2) + a10*16*i + a01*16*j + a00; // /* 16 * j * q2 */ + (16*j)*(a21 * pow((16*i),2) + 2*a12*(16*i)*(16*j) + 3*a03*pow((16*j),2) + (a12 + a11)*(16*i) + (3*a03 + 2*a02) * (16*j) + (a03 + a02 + a01) + /*16*j * q1 */ ((16 * j) * (2*a12 * 16*i + 6*a03 * 16*j + 6*a03 + 2*a02 + 6*a03 * 16*j)));
   
}                        			
