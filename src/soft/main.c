/*
 *
 * SOCLIB_GPL_HEADER_BEGIN
 *
 * This file is part of SoCLib, GNU GPLv2.
 *
 * SoCLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 * SOCLIB_GPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 */


#include <stdio.h>
#include "lm32_sys.h"
#include "../segmentation.h"
#include "etrange.h"

#define N 10
//#define WB_TARGET 0xA1000000


extern char inbyte(void);

mfixed a[10] = {(mfixed) 0,(mfixed) 0, (mfixed) 0,(mfixed) 0,(mfixed) 0,(mfixed) 0,(mfixed) 0,(mfixed) (1<<16),(mfixed) 0,(mfixed) 0};

int fibo(int n);

volatile unsigned long cnt=0;
volatile int VideoInStatus=-1;
unsigned long last_addr=0;
  	int i = 0;
	int j = 0;
	int x = 0;
	int y = 0;


void UART_IrqHandler() {
	cnt ++;
	printf("UART CHARACTER RECV +%c+\n",inbyte());
}

void Videoout_IrqHandler() {
	disable_irq(2);
	*((volatile unsigned long*) VIDEO_OUT_REG) = last_addr;
	printf("VIDEO_OUT IRQ ACK 0x%lx\n",last_addr);
}

void Videoin_IrqHandler() {
	if (VideoInStatus>=0) {
		enable_irq(2);
	}
	//Received Irq, answering
	if (VideoInStatus<=0) {
		*((volatile unsigned long*) VIDEO_IN_REG) = RAM_BASE+0x1000000;
		if (VideoInStatus==0)
			last_addr = RAM_BASE+0x1100000;
		VideoInStatus=1;
		printf("VIDEO_IN IRQ ACK 0x%x\n",RAM_BASE+0x1000000);
	} else {
		*((volatile unsigned long*) VIDEO_IN_REG) = RAM_BASE+0x1100000;
		VideoInStatus=0;
		last_addr = RAM_BASE + 0x1000000;
		printf("VIDEO_IN IRQ ACK 0x%x\n",RAM_BASE+0x1100000);
	}
}

void Coproc_IrqHandler() {

  i++;
  if(i == 40)
    {
      j++;
      i = 0;
      if(j == 30)
	{
	  i=0;
	  j=0;
	}
    }
  x = 16 * i;
  y = 16 * j;
  etrange_polyinit(&s1, &r1, &r2, &q1, &q2, &q3, nmfixed(x), nmfixed(y));
    
}

void main()
{

	volatile int OldVideoInStatus = -1;
	//a[10] = {0, 0, 0, 0, 0, 0, 0, 1, 0, 0}; 
	etrange_initialize(&p0, &s0, &r0, &q0, &r2_c1, &r2_c2, &q2_c1, &q2_c2); 
	//Test interrupt UART
	RegisterIrqEntry(0,UART_IrqHandler);
	//Videoin interrupt
	RegisterIrqEntry(1,Videoin_IrqHandler);
	Videoin_IrqHandler();
	//Videoout interrupt
	RegisterIrqEntry(2,Videoout_IrqHandler);
	disable_irq(2);
	//Enable global interrupts
	unsigned long mask = 1;
	asm volatile("wcsr IE,%0" ::"r"(mask));
	
	while (cnt<5) {
		if ((OldVideoInStatus==-1)&&(VideoInStatus==0)) {
			Videoout_IrqHandler();
			OldVideoInStatus=0;
		}

	}

	return 0;

}

