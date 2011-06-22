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


int fibo(int n);

volatile unsigned long cnt=0;
volatile int VideoInStatus=-1;
unsigned long last_addr=0;

void UART_IrqHandler() {
	cnt ++;
	printf("UART CHARACTER RECV +%c+\n",inbyte());
}

void Videoout_IrqHandler() {
//	disable_irq(2);
	*((volatile unsigned long*) VIDEO_OUT_REG) = last_addr;
	printf("VIDEO_OUT IRQ ACK 0x%lx\n",last_addr);
}

void Videoin_IrqHandler() {
	if (VideoInStatus>=0) {
		enable_irq(2);
	}
	//Received Irq, anwsering
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

int main(void)
{
	P0 p0;
	P1 p1;
	P2 p2;
	P3 p3;

	int j;
	volatile int OldVideoInStatus = -1;
	
//        *((volatile unsigned long*) VIDEO_IN_REG) = 0x55555555;

	for (j=0;j<10;j++) {
		p3.a[j]=(mfixed) 0;
	}

	etrange_initialize(&p3,&p2,&p1,&p0);
	//Test interrupt UART
	RegisterIrqEntry(0,UART_IrqHandler);
	//Videoin interrupt
	RegisterIrqEntry(1,Videoin_IrqHandler);
	Videoin_IrqHandler();
	//Videoout interrupt
	RegisterIrqEntry(2,Videoout_IrqHandler);
	disable_irq(2);
	//Enable global interrutps
	unsigned long mask = 1;
	asm volatile("wcsr IE,%0" ::"r"(mask));
	
//	printf("Read video_in 0x%x\n",*((volatile unsigned int*) 0xA2000000));
//	printf("Write OxAB... Return value: ");
//	*((volatile unsigned int*) 0xA2000000) = 0xAB;
//
//	printf("0x%x\n",*((volatile unsigned int*) 0xA2000000));
//	printf("Read video_in 0x%x\n",*((volatile unsigned int*) 0xA2000000));


	while (cnt<5) {
		if ((OldVideoInStatus==-1)&&(VideoInStatus==0)) {
			Videoout_IrqHandler();
			OldVideoInStatus=0;
		}

	}

	return 0;

	//mfixed A,B,C,D;
	int i;
	int  fibov[N], fibot[N];
	fibot[0] = get_cc();
	for (i = 1; i < N; i++)
	{
		fibov[i] = fibo(i);fibot[i] = get_cc();
	}
	fibov [0]= get_cc();


	for (i = 1; i < 10; i++)
		printf(" Fibo %d : %d at %d\n",i,fibov[i],fibot[i]-fibot[i-1]);

	printf("End  time %d\n\n", fibov[0]);

	printf("Number of correct received requests: %d\n",*((volatile unsigned int*) WB_TARGET));
	printf("Number of correct received requests: %d\n",*((volatile unsigned int*) WB_TARGET));
	printf("\nSending write request to simple_slave...\n");
	*((volatile unsigned int*) WB_TARGET) = 123;
	printf("Number of correct received requests: %d\n",*((volatile unsigned int*) WB_TARGET));
	printf("\nSending write request to simple_slave...\n");
	*((volatile unsigned int*) WB_TARGET) = 123;
	printf("Number of correct received requests: %d\n",*((volatile unsigned int*) WB_TARGET));
	printf("Number of correct received requests: %d\n",*((volatile unsigned int*) WB_TARGET));
	printf("\nSending write request to simple_slave...\n");
	*((volatile unsigned int*) WB_TARGET) = 123;
	printf("Number of correct received requests: %d\n",*((volatile unsigned int*) WB_TARGET));

	printf("\nDone.\n");

	getchar();
	return 0;
}


int fibo(int n)
{
	if (n==0)
		return 1;
	else if (n==1)
		return 1;
	else return fibo(n-1) + fibo(n-2);
}

