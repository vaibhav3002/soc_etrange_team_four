/*
 *
 * SOCLIB_LGPL_HEADER_BEGIN
 * 
 * This file is part of SoCLib, GNU LGPLv2.1.
 * 
 * SoCLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * SoCLib is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with SoCLib; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * SOCLIB_LGPL_HEADER_END
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2006-2007
 *
 * Maintainers: tarik.graba@telecom-paristech.fr
 */

/////////////////////////////////////////////////////////////////
//	ADDRESS SPACE SEGMENTATION
//
//	This file must be included in the system.cpp file, 
//	for harware configuration : It is used to build
//	the SOCLIB_SEGMENT_TABLE.
//
//	This file is also used by the ldscript generator,
//	for embedded software generation.
//	
//	It gives the system integrator the garanty
//	that hardware and software have the same
//	description of the address space segmentation.
//	
//	The segment names cannot be changed.
/////////////////////////////////////////////////////////////////

#define	ROM_BASE    0x00000000
#define	ROM_SIZE    0x00100000 // 1M
#define	RAM_BASE    0x40000000 
#define	RAM_SIZE    0x02000000 // 32M
#define	TTY_BASE    0xA0000000
#define	TTY_SIZE    0x00000100

#define  WBM_BASE    0xB1000000
#define  WBM_SIZE    0X00000010

//Coprocessor base and size
#define  COPROC_BASE    0xF0000000
#define  COPROC_SIZE    0X00000128

//defining data write mem
#define WB_TARGET    0xB0000000

//video in register adress
#define VIDEO_IN_REG 0xC0000000

//video_out register adress
#define VIDEO_OUT_REG 0xD0000000
//interpolator_out_reg
#define INTERPOLATOR_OUT_REG 0xE0000000
//BUFFER MANAGEMENT OUT REG
#define BUFFER_MANAGEMENT_REG 0xE0000001
