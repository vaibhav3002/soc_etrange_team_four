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
#include <string.h>
#include "lm32_sys.h"
#include "../segmentation.h"
#include "etrange.h"

#define N 10
//#define WB_TARGET 0xA1000000

#define LOGO_W 60
#define LOGO_H 60
#define LOGO_OFFSET 0x3CEA6

extern volatile unsigned long _binary___logo_telecom_pgm_start; 

extern char inbyte(void);

char logo = 1;

volatile unsigned long cnt=0;
volatile int VideoInStatus=-1;
unsigned long last_addr=0;

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
	int i,j,OldStatus;
	OldStatus=VideoInStatus;
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
	if ((OldStatus>=0)&&(logo==1)) {
//		unsigned long temp1,temp2;
		for (i=0; i<LOGO_H; i++) {
			memcpy((void*) (last_addr+i*(640)),(void*) (&_binary___logo_telecom_pgm_start)+i*(LOGO_W),LOGO_W);
/*			for (j=0; j<LOGO_W/4; j++) {
				temp1 =*((&_binary___logo_telecom_pgm_start)+j+i*(LOGO_W/4));
				temp2 =((temp1&0xFF)<<24) | ((temp1&0xFF00) <<8) | ((temp1 & 0xFF0000) >> 8) | ((temp1 & 0xFF000000) >> 24);
				*((volatile unsigned long*) last_addr+i*(640/4)+j)=temp2;
			}*/
		}
		
	}

	if (VideoInStatus>=0) {
		enable_irq(2);
	}

}

void Coproc_IrqHandler() {
	
}


int main(void)
{
	P0 p0;
	P1 p1;
	P2 p2;
	P3 p3;

	int j;
	volatile int OldVideoInStatus = -1;

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
	//Coproc interrupt
	RegisterIrqEntry(3,Coproc_IrqHandler);
	disable_irq(3);
	//Enable global interrupts
	unsigned long mask = 1;
	asm volatile("wcsr IE,%0" ::"r"(mask));
	
	while (cnt<5) {
		if ((OldVideoInStatus==-1)&&(VideoInStatus==0)) {
			for (j=0;j<1000;j++) {}
			Videoout_IrqHandler();
			OldVideoInStatus=0;
		}

	}

	return 0;

}

