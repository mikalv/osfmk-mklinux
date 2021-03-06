/*
 * Copyright 1991-1998 by Open Software Foundation, Inc. 
 *              All Rights Reserved 
 *  
 * Permission to use, copy, modify, and distribute this software and 
 * its documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appears in all copies and 
 * that both the copyright notice and this permission notice appear in 
 * supporting documentation. 
 *  
 * OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
 * FOR A PARTICULAR PURPOSE. 
 *  
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR 
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT, 
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. 
 */
/*
 * MkLinux
 */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * Copyright (c) 1991 Sequent Computer Systems
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON AND SEQUENT COMPUTER SYSTEMS ALLOW FREE USE OF
 * THIS SOFTWARE IN ITS "AS IS" CONDITION.  CARNEGIE MELLON AND
 * SEQUENT COMPUTER SYSTEMS DISCLAIM ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */

/* CMU_HIST */
/*
 * Revision 2.3  91/07/31  18:00:07  dbg
 * 	Changed copyright.
 * 	[91/07/31            dbg]
 * 
 * Revision 2.2  91/05/08  12:54:26  dbg
 * 	Added, from Sequent SYMMETRY sources.
 * 	[91/04/26  14:49:30  dbg]
 * 
 */

/*
 * $Header: /u1/osc/rcs/mach_kernel/sqt/chiptypes.h,v 1.2.10.1 1994/09/23 03:03:54 ezf Exp $
 *
 * chiptypes.h
 *	Definitions of Intel chips on the processor board.
 *
 */

/*
 * Revision 1.1  89/07/05  13:15:27  kak
 * Initial revision
 * 
 * Revision 1.2  88/10/25  09:32:21  rto
 * Repaired typo: CHIPTYEPS --> CHIPTYPES.
 * 
 * Revision 1.1  88/10/10  15:09:27  rto
 * Initial revision
 * 
 * Revision 1.3  88/09/13  11:29:13  rto
 * 532port:  Added #ifndef __CHIPTYPES__.
 * 
 * Revision 1.2  88/08/02  08:34:53  rto
 * 532port:  Added copyright and header information.
 * 
 *
 */


/*
 * Intel components on the processor board
 */
#ifndef __CHIPTYPES__

#ifdef i386
#define CPU_TYPE  i386
#define MMU_TYPE  i386 
#define FPU_TYPE  i387
#endif i386

#define __CHIPTYPES__
#endif __CHIPTYPES__

