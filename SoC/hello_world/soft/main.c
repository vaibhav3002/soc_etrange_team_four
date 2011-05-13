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


#define N 10
#define WB_TARGET 0xA1000000

int fibo(int n);

int main(void)
{
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

