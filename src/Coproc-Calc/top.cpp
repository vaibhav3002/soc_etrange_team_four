#include "systemc.h"
#include "incr_calc.h"

/*extern "C"
{
#include "mfixed.h"
}*/
#include <cmath>

void next_cycle (sc_signal<bool> &signal_clk);
void calc_coeffs (int i, int j);

mfixed a30 = (mfixed(pow(2,-15)));
//mfixed(0,3051757);//pow(2,-15);
mfixed a20 = (mfixed(pow(-3.2,-8)));//mfixed(0,9094947);//
mfixed a10 = (mfixed(2));
mfixed a00 = (mfixed(0));
mfixed a21 = (mfixed(0));
mfixed a11 = (mfixed(pow(-2,-7)));//mfixed(-0,78125000);//pow(-2,-7);
mfixed a01 = (mfixed(1));
mfixed a12 = (mfixed(pow(2,-15)));// pow(2,-15);
mfixed a02 = (mfixed(pow(-2,-8)));//mfixed(0,390625000);//pow(-2,-8);
mfixed a03 = (mfixed(0));

    sc_signal<mfixed> p0;
    sc_signal<mfixed> p1;
    sc_signal<mfixed> p2;
    sc_signal<mfixed> p3;
    sc_signal<mfixed> q0;
    sc_signal<mfixed> q1;
    sc_signal<mfixed> q2;
    sc_signal<mfixed> q3;
    sc_signal<mfixed> r0;
    sc_signal<mfixed> r1;
    sc_signal<mfixed> r2;
sc_signal<mfixed> r2_c1;
sc_signal<mfixed> r2_c2;
sc_signal<mfixed> q1_c1;
sc_signal<mfixed> q1_c2;
sc_signal<mfixed> q2_c2;
sc_signal<mfixed> q2_c1;
sc_signal<unsigned short> o_x;
sc_signal<short> o_dx;

mfixed x2, y2, x3, y3;

    sc_signal<mfixed> s0;
    sc_signal<mfixed> s1;

int sc_main(int argc, char *argv[])
{
    using namespace sc_core;
    using namespace soclib::caba;    


    int i, j;
    mfixed x, y;
    mfixed x_norm, y_norm;
    mfixed x_exp;

    sc_signal<bool> clk;
    sc_signal<bool> en;
    sc_signal<bool> reset_n;
/*    sc_signal<mfixed> x_3;
    sc_signal<mfixed> x_2;
    sc_signal<mfixed> x_1; */
    sc_signal<bool> x_valid;
    sc_signal<bool> start;
    sc_signal<bool> finished;



    sc_trace_file *my_trace_file;
    my_trace_file =
        sc_create_vcd_trace_file ("incr_calc_trace");
    sc_trace(my_trace_file, clk, "clk");
    sc_trace(my_trace_file, reset_n, "reset");
    sc_trace(my_trace_file, x_valid, "valid");
    sc_trace(my_trace_file, finished, "finished");

    IncrCalc my_calc("coproc_incr");
    my_calc.clk(clk);
    my_calc.reset_n(reset_n);
    //my_calc.x_3(x_3);
    my_calc.en(en);
/*    my_calc.x_2(x_2);
      my_calc.x_1(x_1);*/
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
    my_calc.start(start);
    my_calc.dx(o_dx);
    my_calc.x(o_x);
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

    x = mfixed(0);
    y = mfixed(0);
    x_exp = mfixed(0);


    x2 = fx_mul(x,x);
    x3 = fx_mul(x2,x);

    y2 = fx_mul(y,y);
    y3 = fx_mul(y2,y);

    cout << "not enabled !! " << endl;
    en = 0;
    cout << " -------------- " << endl;
    cout << x_exp << " " << mfixed(o_dx,o_x) << endl;
    next_cycle(clk);
    cout << " -------------- " << endl;
    cout << x_exp << " " << mfixed(o_dx,o_x) << endl;
    next_cycle(clk);
    cout << " -------------- " << endl;
    cout << x_exp << " " << mfixed(o_dx,o_x) << endl;
    next_cycle(clk);
    cout << " -------------- " << endl;
    cout << x_exp << " " << mfixed(o_dx,o_x) << endl;
    next_cycle(clk);
	     
    en = 1;

    //for each tile
    for(j=0; j<30; j++) //30 vertically
	for(i=0; i<40; i++) //40 horizontally
	{
	    x = fx_mul(mfixed(TILE_WIDTH),mfixed(i));
	    y = fx_mul(mfixed(TILE_HEIGHT),mfixed(j));
	    cout << "x : " << x << " y: " << y << endl;
	    calc_coeffs(i, j);

	    start = 1;
	    next_cycle(clk);
	    en = 1;
	    start = 0;
	    x_exp = mfixed(0);
	    next_cycle(clk);

	    while(x_valid && !finished)
	    {
	      //x_exp = a30 * pow(x,3) + a21 * pow(x,2) * y + a12 * x * pow(y,2) + a03 * pow(y,3) + a20 * pow(x,2) + a11 * x * y + a02 * pow(y,2) + a10 * x + a01 * y + a00;
	      x_exp = fx_add(fx_mul(a30,x3),fx_add(fx_mul(a21,fx_mul(x2,y)),fx_add(fx_mul(a12,fx_mul(x,y2)),fx_add(fx_mul(a03,y3),fx_add(fx_mul(a20,x2),fx_add(fx_mul(a11,fx_mul(x,y)),fx_add(fx_mul(a02,y2),fx_add(fx_mul(a10,x),fx_add(fx_mul(a01,y),a00)))))))));
		cout << "x : " << x << " y: " << y << endl;
		next_cycle(clk);
		cout << " -------------- " << endl;
		cout << x_exp << " " << mfixed(o_dx,o_x) << endl;
		if((to_float(mfixed(o_dx,o_x)) - to_float(x_exp))>0.03*to_float(mfixed(o_dx,o_x)))
		    cout << "ERROR" << endl;
		x = fx_add(x,mfixed(1));
		if (x == (fx_add(mfixed(TILE_WIDTH),(fx_mul(mfixed(16),mfixed(i))))))
		{
		    x = fx_mul(mfixed(16),mfixed(i));
		    y = fx_add(y,mfixed(1));
		}

		cout << "x " << o_x << "dx " << o_dx << endl;

		x2 = fx_mul(x,x);
		x3 = fx_mul(x2,x);

		y2 = fx_mul(y,y);
		y3 = fx_mul(y2,y);

		//cout << x2 << y2 << endl;

		//cout << "mul test" << fx_mul(mfixed(1.5),mfixed(3.5)) << endl;
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
    s0 = fx_mul(mfixed(2),(a21));
    r0 = fx_mul(mfixed(2),(a12));
    q0 = fx_mul(mfixed(6),(a03));
    p0 = fx_mul(mfixed(6),a30);

    mfixed x = fx_mul(mfixed(16),mfixed(i));
    mfixed y = fx_mul(mfixed(16),mfixed(j));

    r2_c1 = fx_add(fx_mul(mfixed(3),a30),fx_mul(mfixed(2),a20));
       
    r2_c2 = fx_add(a21,a11);

       
    q2_c1 = fx_add(a12,a11);
      
    q2_c2 = fx_add(fx_mul(mfixed(3),a03),fx_mul(mfixed(2),a02));

    cout << "foo" << endl;
    
    /*      c00          + 16*i*c10       + 16*j*c01         + s0 * j */
    //s1 = (6*a30 + 2*a20) + 6*a30 * x + 2 * a21 * y;// + 2*a21 * 16 * j;


  s1 = fx_add(fx_mul(mfixed(6),a30),fx_add(fx_mul(mfixed(2),a20),fx_add(fx_mul(mfixed(6),fx_mul(a30,x)),fx_mul(mfixed(2),fx_mul(a21,y)))));

    /*    r1                                           +   r0 * 16  */
    //r1 = 2*a12 * x + 2*a12 * y + a12 + a21 + a11 + 2*a12 * y;
  r1 = fx_add(fx_mul(mfixed(2),fx_mul(a12,x)),fx_add(fx_mul(mfixed(2),fx_mul(a12,y)),fx_add(a12,(fx_add(a21,(fx_add(a11,(fx_mul(mfixed(2),fx_mul(a12,y))))))))));

    //mfixed r2_max = pow((IMAGE_WIDTH),2) * 3*a30 + 2*a21*IMAGE_HEIGHT*IMAGE_WIDTH + a12 * pow((IMAGE_HEIGHT),2) + (3*a30+2*a20)*IMAGE_WIDTH + (a21+a11) * IMAGE_HEIGHT + (a30 + a20 + a10); 

    //r2 = pow((x),2) * 3*a30 + 2*a21*y*x + a12 * pow((y),2) + (3*a30+2*a20)*x + (a21+a11) * y + (a30 + a20 + a10); //+ /*16*j*r1*/ (16*j) * (2*a12 * 16*i + 2*a12 * 16*j + a12 + a21 + a11 + 2*a12 * 16*j);
  r2 = fx_add(fx_mul(x2,fx_mul(mfixed(3),a30)),fx_add(fx_mul(mfixed(2),fx_mul(a21,fx_mul(y,x))),fx_add(fx_mul(a12,y2),fx_add(fx_mul(r2_c1,x),fx_add(fx_mul(r2_c2,y),fx_add(a30,(fx_add(a20,(a10)))))))));

    //r2 = r2 / r2_max;

	     //   q1 = 2*a12 * x + 6*a03 * y + 6*a03 + 2*a02 + 6*a03 * y;
	     q1 = fx_add(fx_mul(mfixed(2),fx_mul(a12,x)),fx_add(fx_mul(mfixed(6),fx_mul(a03,y)),fx_add(fx_mul(mfixed(6),a03),fx_add(fx_mul(mfixed(2),a02),fx_mul(mfixed(6),fx_mul(a03,y))))));

    //float q2_max = a21 * pow((IMAGE_WIDTH),2) + 2*a12*(IMAGE_WIDTH)*(IMAGE_HEIGHT) + 3*a03*pow((IMAGE_HEIGHT),2) + (a12 + a11)*(IMAGE_WIDTH) + (3*a03 + 2*a02) * (IMAGE_HEIGHT) + (a03 + a02 + a01) + /*16*j * q1 */ (IMAGE_HEIGHT) * (2*a12 * IMAGE_WIDTH + 6*a03 * IMAGE_HEIGHT + 6*a03 + 2*a02 + 6*a03 * IMAGE_HEIGHT);

	     // q2 = a21 * pow((x),2) + 2*a12*(x)*(y) + 3*a03*pow((y),2) + (a12 + a11)*(x) + (3*a03 + 2*a02) * (y) + (a03 + a02 + a01);// + /*16*j * q1 */ (y) * (2*a12 * x + 6*a03 * y + 6*a03 + 2*a02 + 6*a03 * y);
	     q2 = fx_add(fx_mul(a21,x2),fx_add(fx_mul(mfixed(2),fx_mul(a12,fx_mul(x,y))),fx_add(fx_mul(mfixed(3),fx_mul(a03,y2)),fx_add(fx_mul(q2_c1,x),fx_add(fx_mul(q2_c2,y),fx_add(a03,(fx_add(a02,(a01)))))))));

    //q2 = q2/q2_max;

    //q3 = a30 * pow((x),3) + a21 * pow((x),2) * y + a12 * x * pow((y),2) + a03*pow((y),3) + a20*pow((x),2) + a11*x*y + a02*pow((y),2) + a10*x + a01*y + a00;
	     q3 = fx_add(fx_mul(a30,x3),fx_add(fx_mul(a21,fx_mul(x2,y)),fx_add(fx_mul(a12,fx_mul(x,y2)),fx_add(fx_mul(a03,y3),fx_add(fx_mul(a20,x2),fx_add(fx_mul(a11,fx_mul(x,y)),fx_add(fx_mul(a02,y2),fx_add(fx_mul(a10,x),fx_add(fx_mul(a01,y),a00)))))))));

    //float q3_max =a30 * pow((IMAGE_WIDTH),3) + a21 * pow((IMAGE_WIDTH),2) * IMAGE_HEIGHT + a12 * IMAGE_WIDTH * pow((IMAGE_HEIGHT),2) + a03*pow((IMAGE_HEIGHT),3) + a20*pow((IMAGE_WIDTH),2) + a11*IMAGE_WIDTH*IMAGE_HEIGHT + a02*pow((IMAGE_HEIGHT),2) + a10*IMAGE_WIDTH + a01*IMAGE_HEIGHT + a00;

    //q3 = q3/q3_max;
 // /* 16 * j * q2 */ + (16*j)*(a21 * pow((16*i),2) + 2*a12*(16*i)*(16*j) + 3*a03*pow((16*j),2) + (a12 + a11)*(16*i) + (3*a03 + 2*a02) * (16*j) + (a03 + a02 + a01) + /*16*j * q1 */ ((16 * j) * (2*a12 * 16*i + 6*a03 * 16*j + 6*a03 + 2*a02 + 6*a03 * 16*j)));
   
}                        			
